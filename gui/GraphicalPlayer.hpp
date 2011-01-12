/*
 * gui/GraphicalPlayer.hpp
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

#ifndef LM_GUI_GRAPHICALPLAYER_HPP
#define LM_GUI_GRAPHICALPLAYER_HPP

#include "common/Player.hpp"
#include "GraphicContainer.hpp"
#include "DrawContext.hpp"
#include "Bone.hpp"
#include <string>
#include "Sprite.hpp"

namespace LM {
	class ResourceCache;

	class GraphicalPlayer : public Player {
	public:
		enum WeaponPartId {
			PART_BACK_HAND = 0,
			PART_FRONT_HAND = 1,

			PART_FRONT_ARM = 0,
			PART_BACK_ARM = 2,

			PART_UNFIRED = 0,
			PART_FIRED = 4,

			PART_MAX = 8
		};
	private:
		GraphicContainer m_graphic_root;
		Bone m_root_bone;
		ResourceCache* m_cache;

		PixelShader m_blur_shader;
		ShaderSet* m_blur;

	protected:
		virtual void update_location();

	public:
		GraphicalPlayer(const char* name, uint32_t id, char team, ResourceCache* cache);
		~GraphicalPlayer();

		GraphicContainer* get_graphic();
		Bone* get_bone();

		virtual void set_position(float x, float y);
		virtual void set_rotation_degrees(float rotation);
		virtual void set_gun_rotation_degrees(float rotation);
		virtual void set_is_invisible(bool invisible);
		virtual void set_is_frozen(bool is_frozen, int freeze_time = 0);

		Graphic* get_weapon_graphic(int partid);
	};
}

#endif
