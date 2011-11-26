/*
 * gui/SimpleLineEmitter.cpp
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

#include "SimpleLineEmitter.hpp"
#include "Particle.hpp"
#include <math.h>

using namespace LM;
using namespace std;

SimpleLineEmitter::SimpleLineEmitter(ParticleManager* manager, Point center, Image* image, DrawContext::BlendMode mode) : ParticleEmitter(manager, center, image, mode) {
	m_lifetime = 0;
	m_spawned_total = 0;
}

SimpleLineEmitter::~SimpleLineEmitter() {
}

void SimpleLineEmitter::init(const SimpleLineEmitterSettings* settings) {
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
	
	m_leftover_diff = 0;
	m_lifetime = 0;
}

bool SimpleLineEmitter::update(uint64_t timediff) {
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

void SimpleLineEmitter::init_particle(Particle* particle) {
	Vector end_offset = (m_endpoint - get_center());
	particle->m_pos = get_center() + end_offset * (rand()/(float)RAND_MAX);
	particle->m_prev_pos = particle->m_pos;
	float speed = m_settings->particle_speed + (rand()/(float)RAND_MAX) * m_settings->speed_variance;
	float dir = end_offset.get_angle() + (rand()/(float)RAND_MAX) * m_settings->rotation_variance - m_settings->rotation_variance/2;
	
	particle->m_vel = Vector(speed * sin(dir), speed * cos(dir));
	
	particle->m_energy_left = m_settings->lifetime_millis + rand() % m_settings->lifetime_variance;
	particle->m_initial_energy = particle->m_energy_left;
	
	particle->m_color.r = 255;
	particle->m_color.g = 255;
	particle->m_color.b = 255;
	particle->m_color.a = 255;
	
	particle->m_size = 1.0f;
}

void SimpleLineEmitter::set_endpoint(Point point) {
	m_endpoint = point;
}

void SimpleLineEmitter::set_endpoint(float x, float y) {
	m_endpoint.x = x;
	m_endpoint.y = y;
}

