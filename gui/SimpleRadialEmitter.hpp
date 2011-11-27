/*
 * gui/SimpleRadialEmitter.hpp
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

#ifndef LM_GUI_SIMPLERADIALEMITTER_HPP
#define LM_GUI_SIMPLERADIALEMITTER_HPP

namespace LM {
	struct SimpleRadialEmitterSettings {
		float particle_speed;
		float speed_variance;
		int spawn_per_second;
		int spawn_variance;
		uint64_t lifetime_millis;
		uint64_t lifetime_variance;
		float rotation_rads;
		float rotation_variance;
		Vector global_force;
		int max_spawn;
		uint64_t emitter_stop_spawning_millis;
		uint64_t emitter_lifetime_millis;
	};

	class SimpleRadialEmitter : public ParticleEmitter {
	private:
		uint64_t m_lifetime;
		uint64_t m_leftover_diff;
		int m_spawned_total;
		float m_curr_rotation;
	
		const SimpleRadialEmitterSettings* m_settings;
		bool m_delete_settings;
		
		void init_particle(Particle* particle);
	public:
		SimpleRadialEmitter(ParticleManager* manager, Point center, Image* image, DrawContext::BlendMode mode = DrawContext::BLEND_ADD);
		virtual ~SimpleRadialEmitter();
		
		void init(const SimpleRadialEmitterSettings* settings, bool delete_settings = false);
		
		virtual bool update(uint64_t timediff);
		
		static SimpleRadialEmitterSettings* parse_settings_string(std::string settings_string);
		
		static void parse_param(SimpleRadialEmitterSettings* settings, std::string name, std::string value);
		
		void set_rotation(float rads);
	};
}

#endif
