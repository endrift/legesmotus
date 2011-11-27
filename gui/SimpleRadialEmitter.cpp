/*
 * gui/SimpleRadialEmitter.cpp
 *
 * This file is part of Leges Motus, a networked, 2D shooter set in zero gravity.
 * 
 * Copyright 2009-2011 Andrew Ayer, Nathan Partlan, Jeffrey Pfau
 * 
 * Leges Motus is free and open source software.  You may redistribute it and/or
 * modify it under the terms of version 2, or (at your option) version 3, of the
 * GNU General Public License (GPL), as published by the Free Software Foundation.
 * 
 * Leges Motus is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
 * PARTICULAR PURPOSE.  See the full text of the GNU General Public License for
 * further detail.
 * 
 * For a full copy of the GNU General Public License, please see the COPYING file
 * in the root of the source code tree.  You may also retrieve a copy from
 * <http://www.gnu.org/licenses/gpl-2.0.txt>, or request a copy by writing to the
 * Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 * 02111-1307  USA
 * 
 */

#include "SimpleRadialEmitter.hpp"
#include "Particle.hpp"
#include <math.h>

using namespace LM;
using namespace std;

SimpleRadialEmitter::SimpleRadialEmitter(ParticleManager* manager, Point center, Image* image, DrawContext::BlendMode mode) : ParticleEmitter(manager, center, image, mode) {
	m_lifetime = 0;
	m_spawned_total = 0;
	m_settings = NULL;
	m_delete_settings = false;
	m_curr_rotation = 0;
	m_leftover_diff = 0;
}

SimpleRadialEmitter::~SimpleRadialEmitter() {
	if (m_delete_settings) {
		delete m_settings;
	}
}

void SimpleRadialEmitter::init(const SimpleRadialEmitterSettings* settings, bool delete_settings) {
	if (m_delete_settings) {
		delete m_settings;
	}
	
	m_delete_settings = delete_settings;

	m_settings = settings;
	
	// Initialize the vertex/color arrays to be at least the max expected particles
	if (m_settings->max_spawn > 0) {
		init_arrays(m_settings->max_spawn + 1);
	} else if (m_settings->emitter_stop_spawning_millis > 0) {
		init_arrays(m_settings->emitter_stop_spawning_millis * (m_settings->spawn_per_second/1000.0) + 1);
	} else if (m_settings->emitter_lifetime_millis > 0) {
		init_arrays(m_settings->emitter_lifetime_millis * (m_settings->spawn_per_second/1000.0) + 1);
	} else {
		init_arrays(MAX_PARTICLES_AT_A_TIME);
	}
}

bool SimpleRadialEmitter::update(uint64_t timediff) {
	timediff += m_leftover_diff;
	
	if (timediff <= 0) {
		return true;
	}
	
	m_leftover_diff = 0;
	int num_to_spawn = int(m_settings->spawn_per_second * timediff/1000.0 + rand() % m_settings->spawn_variance * timediff/1000.0);
	
	if (num_to_spawn == 0) {
		m_leftover_diff += timediff;
	}
	
	// Spawn new particles
	if (m_settings->emitter_stop_spawning_millis <= 0 || m_settings->emitter_stop_spawning_millis > m_lifetime) {
		for (int i = 0; i < num_to_spawn; i++) {
			if (m_particles.size() >= MAX_PARTICLES_AT_A_TIME ||
				(m_settings->max_spawn > 0 && m_spawned_total >= m_settings->max_spawn)) {
				break;
			}
		
			Particle* particle = request_particle();
	
			init_particle(particle);
	
			m_particles.push_back(particle);
			m_spawned_total++;
			if (particle == NULL) {
				DEBUG("Could not spawn particle!");
				break;
			}
		}
	}
	
	// Update existing particles
	list<Particle*>::iterator it = m_particles.begin();
	while (it != m_particles.end()) {
		Particle* particle = *it;
		
		if (particle->m_energy_left < timediff) {
			free_particle(particle, it++);
			continue;
		}
		particle->m_energy_left -= timediff;
		
		particle->m_color.a = particle->m_energy_left / float(particle->m_initial_energy);
		
		particle->m_prev_pos = particle->m_pos;
		particle->m_vel += m_settings->global_force * timediff/1000.0f;
		particle->m_pos += particle->m_vel * timediff/1000.0f;
		
		it++;
	}
	
	// Check if we're done spawning/living
	if (m_settings->max_spawn > 0) {
		if (m_spawned_total > m_settings->max_spawn && m_particles.size() <= 0) {
			return false;
		}
	}
	
	m_lifetime += timediff;
	
	if (m_settings->emitter_lifetime_millis > 0) {
		if (m_lifetime > m_settings->emitter_lifetime_millis) {
			return false;
		}
	}
	
	return true;
}

void SimpleRadialEmitter::init_particle(Particle* particle) {
	particle->m_pos = get_center();
	particle->m_prev_pos = particle->m_pos;
	float speed = m_settings->particle_speed + (rand()/(float)RAND_MAX) * m_settings->speed_variance;
	float dir = m_curr_rotation + m_settings->rotation_rads + (rand()/(float)RAND_MAX) * m_settings->rotation_variance - m_settings->rotation_variance/2;
	
	particle->m_vel = Vector(speed * cos(dir), speed * sin(dir));
	
	particle->m_energy_left = m_settings->lifetime_millis + rand() % m_settings->lifetime_variance;
	particle->m_initial_energy = particle->m_energy_left;
	
	particle->m_color.r = 255;
	particle->m_color.g = 255;
	particle->m_color.b = 255;
	particle->m_color.a = 255;
	
	particle->m_size = 1.0f;
}

SimpleRadialEmitterSettings* SimpleRadialEmitter::parse_settings_string(string settings_string) {
	SimpleRadialEmitterSettings* settings = new SimpleRadialEmitterSettings();
	
	while(settings_string.length() > 0) {
		int loc = settings_string.find("|");
		
		if (loc < 0) {
			break;
		}
		
		string token = settings_string.substr(0, loc);
		int equalsloc = token.find("=");
		
		if (equalsloc < 0) {
			continue;
		}
		
		string name = token.substr(0, equalsloc);
		string value = token.substr(equalsloc+1);
		
		parse_param(settings, name, value);
		
		settings_string = settings_string.substr(loc+1);
	}
	
	return settings;
}

void SimpleRadialEmitter::parse_param(SimpleRadialEmitterSettings* settings, string name, string value) {
	if (name == "particle_speed") {
		settings->particle_speed = atof(value.c_str());
	} else if (name == "speed_variance") {
		settings->speed_variance = atof(value.c_str());
	} else if (name == "spawn_per_second") {
		settings->spawn_per_second = atoi(value.c_str());
	} else if (name == "spawn_variance") {
		settings->spawn_variance = atoi(value.c_str());
	} else if (name == "lifetime_millis") {
		settings->lifetime_millis = atoi(value.c_str());
	} else if (name == "lifetime_variance") {
		settings->lifetime_variance = atoi(value.c_str());
	} else if (name == "rotation_rads") {
		settings->rotation_rads = atof(value.c_str());
	} else if (name == "rotation_variance") {
		settings->rotation_variance = atof(value.c_str());
	} else if (name == "max_spawn") {
		settings->max_spawn = atoi(value.c_str());
	} else if (name == "emitter_stop_spawning_millis") {
		settings->emitter_stop_spawning_millis = atoi(value.c_str());
	} else if (name == "emitter_lifetime_millis") {
		settings->emitter_lifetime_millis = atoi(value.c_str());
	}
}

void SimpleRadialEmitter::set_rotation(float rads) {
	m_curr_rotation = rads;
}
