/*
 * client/MapObject.hpp
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

#ifndef LM_CLIENT_MAPOBJECT_HPP
#define LM_CLIENT_MAPOBJECT_HPP

#include "common/Map.hpp"
#include "common/Point.hpp"
#include "common/Polygon.hpp"
#include "client/Graphic.hpp"
#include "client/Sprite.hpp"

/* Represents an object on the map */
namespace LM {
	class MapObject {
	private:
		Map::ObjectType		m_type;			// As defined in the enum in the Map class.
		Point			m_upper_left;
		Graphic*		m_sprite;
		char			m_team;			// 'A', 'B', or 0
		Polygon		m_bounding_polygon;	// Note: coordinates in this polygon are relative to upper left corner of MAP
	
	public:
		MapObject(Map::ObjectType type, Point upper_left);
	
		Map::ObjectType		get_type() const { return m_type; }
		Point			get_upper_left() const { return m_upper_left; }
	
		Graphic*		get_sprite() const { return m_sprite; }
		void			set_sprite(Sprite* sprite);
		void			set_sprite(Graphic* sprite);
		bool			has_sprite() const { return m_sprite != NULL; }
	
		char			get_team() const { return m_team; }
		void			set_team(char team) { m_team = team; }
	
		Polygon&		get_bounding_polygon() { return m_bounding_polygon; }
		const Polygon&	get_bounding_polygon() const { return m_bounding_polygon; }
	
		bool			is_intersectable() const { return m_bounding_polygon.is_filled(); }
		bool			is_obstacle() const { return m_type == Map::SPRITE && is_intersectable(); }
	
	};
	
}

#endif
