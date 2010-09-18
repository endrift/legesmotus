/*
 * gui/GraphicalPlayer.hpp
 *
 * This file is part of Leges Motus, a networked, 2D shooter set in zero gravity.
 * 
 * Copyright 2009-2010 Andrew Ayer, Nathan Partlan, Jeffrey Pfau
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
#include "Bone.hpp"
#include <string>
#include "Sprite.hpp"

namespace LM {
	class ResourceCache;

	class GraphicalPlayer : public Player {
	private:
		GraphicContainer m_graphic_root;
		Bone m_root_bone;

	public:
		GraphicalPlayer(const char* name, uint32_t id, char team, ResourceCache* cache);
		~GraphicalPlayer();

		GraphicContainer* get_graphic();
		Bone* get_bone();

		virtual void set_x(double x);
		virtual void set_y(double y);
		virtual void set_rotation_degrees(double rotation);
	};
}

#endif
