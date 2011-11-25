/*
 * gui/ParticleManager.hpp
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

#include "DrawContext.hpp"
#include "Widget.hpp"
#include <list>

#ifndef LM_GUI_PARTICLEMANAGER_HPP
#define LM_GUI_PARTICLEMANAGER_HPP

namespace LM {
	class Particle;
	class ParticleEmitter;
	
	class ParticleManager : public Widget {
	private:
		std::list<ParticleEmitter*> m_emitters;
		std::list<Particle*> m_free_particles;
		bool m_can_expand_pool;
		int m_total_particles;
	public:
		ParticleManager(Widget* parent, int num_initial_particles, bool can_expand_pool);
		virtual ~ParticleManager();
		
		void add_emitter(ParticleEmitter* emitter);
		void remove_emitter(ParticleEmitter* emitter);
		bool emitter_exists(ParticleEmitter* emitter);
		void update(uint64_t timediff);
		virtual void draw(DrawContext* ctx) const;
		
		Particle* request_particle();
		void free_particle(Particle* particle);
	};
}

#endif
