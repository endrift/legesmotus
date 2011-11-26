/*
 * gui/ParticleEmitter.cpp
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

#include "ParticleEmitter.hpp"
#include "ParticleManager.hpp"
#include "Particle.hpp"
#include "common/misc.hpp"
#include <math.h>

using namespace LM;
using namespace std;

ParticleEmitter::ParticleEmitter(ParticleManager* manager, Point center, Image* image, DrawContext::BlendMode mode) {
	m_manager = manager;
	m_center = center;
	m_image = image;
	m_blend_mode = mode;
	
	vertices = NULL;
	colors = NULL;
}

ParticleEmitter::~ParticleEmitter() {
	if (m_particles.size() > 0) {
		DEBUG("Emitter still has particles remaining...");
	}
	
	delete[] vertices;
	delete[] colors;
}

void ParticleEmitter::init_arrays(int max_expected_particles) {
	if (max_expected_particles <= 0) {
		max_expected_particles = 1;
		DEBUG("Tried to pass negative number into ParticleEmitter::init_arrays.");
	}
	m_max_expected_particles = max_expected_particles;
	vertices = new float[max_expected_particles * 2];
	colors = new float[max_expected_particles * 4];
}

Particle* ParticleEmitter::request_particle() {
	return m_manager->request_particle();
}

void ParticleEmitter::free_particle(Particle* particle, list<Particle*>::iterator it) {
	m_particles.erase(it);
	
	m_manager->free_particle(particle);
}

void ParticleEmitter::draw(DrawContext* context) {
	list<Particle*>::iterator it;
	
	if (vertices == NULL) {
		DEBUG("Vertex array must be initialized.");
	}
	
	if (colors == NULL) {
		DEBUG("Color array must be initialized.");
	}
	
	if (m_image == NULL) {
		DEBUG("Trying to draw emitter with null image.");
		return;
	}
	
	context->push_transform();
	context->bind_image(m_image->get_handle());
	context->set_blend_mode(m_blend_mode);
	
	if (uint32_t(m_max_expected_particles) < m_particles.size()) {
		delete[] vertices;
		delete[] colors;
		init_arrays(max(int(m_particles.size()), m_max_expected_particles*2));
	}
	
	int i = 0;
	for (it = m_particles.begin(); it != m_particles.end(); it++) {
		vertices[2 * i] = (*it)->m_pos.x;
		vertices[(2 * i)+1] = (*it)->m_pos.y;
		//vertices[i+2] = (*it)->m_size * m_image->get_width();
		//vertices[i+3] = (*it)->m_size * m_image->get_height();
		colors[4 * i] = (*it)->m_color.r;
		colors[(4 * i)+1] = (*it)->m_color.g;
		colors[(4 * i)+2] = (*it)->m_color.b;
		colors[(4 * i)+3] = (*it)->m_color.a;
		
		i++;
	}
	context->draw_bound_point_sprites(vertices, m_particles.size(), m_image->get_width(), m_image->get_height(), colors);
	context->unbind_image();
	context->pop_transform();
}

std::list<Particle*>& ParticleEmitter::get_particles() {
	return m_particles;
}

void ParticleEmitter::clear() {
	m_particles.clear();
}

bool ParticleEmitter::update(uint64_t timediff) {
	return false;
}

Point ParticleEmitter::get_center() {
	return m_center;
}

void ParticleEmitter::set_center(Point center) {
	m_center = center;
}

void ParticleEmitter::set_center(float center_x, float center_y) {
	m_center.x = center_x;
	m_center.y = center_y;
}
