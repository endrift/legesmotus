/*
 * gui/ParticleManager.cpp
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

#include "ParticleManager.hpp"
#include "ParticleEmitter.hpp"
#include "Particle.hpp"

using namespace LM;
using namespace std;

ParticleManager::ParticleManager(Widget* parent, int num_initial_particles, bool can_expand_pool) : Widget(parent) {
	for (int i = 0; i < num_initial_particles; i++) {
		m_free_particles.push_back(new Particle());
	}
	
	m_can_expand_pool = can_expand_pool;
	
	m_total_particles = num_initial_particles;
}

ParticleManager::~ParticleManager() {
	list<ParticleEmitter*>::iterator emitterator;
	list<Particle*>::iterator it;
	
	// Free all emitters
	for (emitterator = m_emitters.begin(); emitterator != m_emitters.end(); emitterator++) {
		for (it = (*emitterator)->get_particles().begin(); it != (*emitterator)->get_particles().end(); it++) {
			m_free_particles.push_back((*it));
		}
		
		(*emitterator)->clear();
		
		delete (*emitterator);
	}
	
	m_emitters.clear();
	
	// Free all particles
	for (it = m_free_particles.begin(); it != m_free_particles.end(); it++) {
		delete (*it);
	}
	m_free_particles.clear();
}

void ParticleManager::add_emitter(ParticleEmitter* emitter) {
	if (!emitter_exists(emitter)) {
		m_emitters.push_back(emitter);
	}
}

void ParticleManager::remove_emitter(ParticleEmitter* emitter) {
	if (emitter_exists(emitter)) {
		m_emitters.remove(emitter);
		
		list<Particle*>::iterator it;
	
		for (it = emitter->get_particles().begin(); it != emitter->get_particles().end(); it++) {
			m_free_particles.push_back((*it));
		}
		
		emitter->clear();
	}
}

bool ParticleManager::emitter_exists(ParticleEmitter* emitter) {
	list<ParticleEmitter*>::iterator it;
	
	for (it = m_emitters.begin(); it != m_emitters.end(); it++) {
		if ((*it) == emitter) {
			return true;
		}
	}
	
	return false;
}

Particle* ParticleManager::request_particle() {
	if (m_free_particles.size() == 0) {
		if (!m_can_expand_pool) {
			return NULL;
		}
		
		m_total_particles *= 2;
		for (int i = 0; i < m_total_particles; i++) {
			m_free_particles.push_back(new Particle());
		}
	}
	
	Particle* to_return = m_free_particles.back();
	
	m_free_particles.pop_back();
	
	return to_return;
}

void ParticleManager::free_particle(Particle* particle) {
	m_free_particles.push_back(particle);
}

void ParticleManager::update(uint64_t timediff) {
	list<ParticleEmitter*>::iterator it = m_emitters.begin();
	
	while (it != m_emitters.end()) {
		// Update the emitter, then remove it if it is ready
		if (!(*it)->update(timediff)) {
			list<Particle*>::iterator it2;
			for (it2 = (*it)->get_particles().begin(); it2 != (*it)->get_particles().end(); it2++) {
				m_free_particles.push_back((*it2));
			}
			
			(*it)->clear();
			
			delete *it;
			m_emitters.erase(it++);
		} else {
			it++;
		}
	}
}

void ParticleManager::draw(DrawContext* context) const {
	list<ParticleEmitter*>::const_iterator it;
	
	for (it = m_emitters.begin(); it != m_emitters.end(); it++) {
		//m_context->bind_shader_set(m_emitters->get_shader_program());
		(*it)->draw(context);
		//m_context->unbind_shader_set();
	}
}
