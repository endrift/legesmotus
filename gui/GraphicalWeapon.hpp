/*
 * gui/GraphicalWeapon.hpp
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

#ifndef LM_GUI_GRAPHICALWEAPON_HPP
#define LM_GUI_GRAPHICALWEAPON_HPP

#include "common/ClientWeapon.hpp"
#include "GraphicalPlayer.hpp"

namespace LM {
	class ResourceCache;
	class ParticleManager;
	struct SimpleLineEmitterSettings;
	struct SimpleRadialEmitterSettings;
	
	class GraphicalWeapon : public ClientWeapon {
	private:
		struct GunPart {
			Image image;
			Point position;
			float rotation;
		};

		ResourceCache* m_cache;
		GunPart m_gunpart[GraphicalPlayer::PART_MAX];
		
		std::string m_firing_line_emitter_image_name_red;
		std::string m_firing_line_emitter_image_name_blue;
		SimpleLineEmitterSettings* m_firing_line_emitter_settings;
		
		std::string m_firing_radial_emitter_image_name_red;
		std::string m_firing_radial_emitter_image_name_blue;
		SimpleRadialEmitterSettings* m_firing_radial_emitter_settings;

	public:
		GraphicalWeapon(ResourceCache* cache);
		virtual bool parse_param(const char* param_string, Weapon* owner);
		virtual void select(Player* player);
		
		virtual void generate_fired_emitter(ParticleManager* manager, ResourceCache* cache, float start_x, float start_y, float end_x, float end_y, float rotation, char team);
	};
}

#endif
