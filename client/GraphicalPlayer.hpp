/*
 * client/GraphicalPlayer.hpp
 *
 * This file is part of Leges Motus, a networked, 2D shooter set in zero gravity.
 * 
 * Copyright 2009 Andrew Ayer, Nathan Partlan, Jeffrey Pfau
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

#ifndef LM_CLIENT_GRAPHICALPLAYER_HPP
#define LM_CLIENT_GRAPHICALPLAYER_HPP

#include "common/Player.hpp"
#include "GraphicGroup.hpp"

namespace LM {
	class Sprite;
	class Graphic;
	
	class GraphicalPlayer : public Player {
	private:
		GraphicGroup* 		m_sprite;
		Graphic*		m_name_sprite;
		double 			m_bounding_radius;
	
	public:
		GraphicalPlayer();
		GraphicalPlayer(const char* name, uint32_t id, char team, GraphicGroup* sprite = NULL, double x = 0, double y = 0, double xvel = 0, double yvel = 0, double rotation = 0);
		~GraphicalPlayer();
	
		GraphicGroup* get_sprite() const { return m_sprite; }
		Graphic* get_name_sprite() const { return m_name_sprite; }
		double get_radius() const { return m_bounding_radius; }
		//double get_width();
		//double get_height();
			
		void set_sprite(GraphicGroup* s);
		void set_name_sprite(Graphic* ns);
		void set_radius(double radius);
		void set_is_invisible(bool is_invisible);
		void set_x(double x);
		void set_y(double y);
		void set_rotation_degrees(double rotation);
		void set_rotation_radians(double rotation);
		void set_is_frozen(bool is_frozen);
	};
	
}

#endif
