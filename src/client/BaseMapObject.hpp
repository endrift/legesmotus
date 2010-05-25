/*
 * client/BaseMapObject.hpp
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

#ifndef LM_CLIENT_BASEMAPOBJECT_HPP
#define LM_CLIENT_BASEMAPOBJECT_HPP

#include "common/Point.hpp"

namespace LM {
	class Shape;
	class Player;
	class Graphic;
	class GameController;
	class ClientMap;
	class MapReader;

	class BaseMapObject {
	private:
		Point		m_position;

	public:
		Point		get_position () const { return m_position; }

		// Returns the shape that bounds this map object
		//  May return NULL if this map object has no bounding shape (i.e. it's decoration)
		virtual const Shape*	get_bounding_shape () const = 0;

		// Returns true if the object has a bounding shape, false otherwise
		bool		is_intersectable () const { return get_bounding_shape() != NULL; }

		
		// Returns the graphic for this map object
		//  May return NULL if the map has no graphic
		virtual Graphic*	get_graphic () const = 0;

		// Returns true if the object has a graphic, false otherwise
		bool		has_graphic () const { return get_graphic() != NULL; }


		// Returns true if the player can jump off of this obstacle, false otherwise
		virtual bool	is_jumpable () const = 0;

		// Returns true if shots can hit this obstacle, false if they travel through the obstacle
		virtual bool	is_shootable () const = 0;

		// Returns true if players can collide with this obstacle, false otherwise
		virtual bool	is_collidable () const = 0;

		// Returns true if players can interact with this obstacle while within its bounds, false otherwise
		virtual bool	is_interactive () const = 0;


		// Returns true if the player is currently engaging this obstacle
		virtual bool	is_engaged () const = 0;


		// Called when this obstacle was shot by the given player, at the given point.
		//  (Only called if is_shootable() returns true)
		//  Returns true if the obstacle took the hit, false if the shot was redirected.
		virtual bool	shot (GameController& gc, Player& shooter, Point point_hit, double direction) = 0;

		// Called when the player is colliding with the obstacle
		//  (Only called if is_collidable() returns true)
		//  Colliding means that the player is intersecting the obstacles's _edge_, and moving closer into the obstacle
		//  old_position is the player's position before moving closer into this obstacle
		//  angle_of_incidence is in degrees (+/-90 == horizontal, +/-180 == vertical)
		virtual void	collide (GameController& gc, Player& player, Point old_position, double angle_of_incidence) = 0;

		// Called every frame during which a player is _within_ this obstacle's bounds
		//  (Only called if is_interactive() returns true)
		//  is_engaged() should return true after this function returns
		virtual void	interact (GameController& gc, Player& player) = 0;

		// Called the first frame that the player is no longer within the obstacle's bounds
		//  (as determined by the result of calling is_engaged above)
		//  (Only called if is_interactive() returns true)
		//  is_engaged() should returns false after this function returns
		virtual void	disengage (GameController& gc, Player& player) = 0;

		
		virtual void	init (MapReader& reader, ClientMap& map) = 0;


		explicit BaseMapObject(Point position) : m_position(position) { }
		virtual ~BaseMapObject() { }

		static BaseMapObject*	make_map_object (MapReader& reader, ClientMap& map);
	};
}

#endif
