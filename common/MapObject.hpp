/*
 * common/MapObject.hpp
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

#ifndef LM_COMMON_MAPOBJECT_HPP
#define LM_COMMON_MAPOBJECT_HPP

#include "Point.hpp"
#include "PhysicsObject.hpp"

class b2Shape;
class b2World;
class b2Contact;

namespace LM {
	class Shape;
	class Player;
	class Map;
	class MapReader;
	class ClientMapObject;
	class PhysicsObject;

	class MapObject : public PhysicsObject {
	
	private:
		Point m_position;
		bool m_is_tiled;
		Vector m_tile_dimensions;
		float m_scale_x;
		float m_scale_y;
		float m_rotation;  // in degrees

	protected:
		ClientMapObject* m_clientpart;

		bool parse_param(const char* option_string);

	public:
		enum CollisionResult {
			IGNORE = 0,
			BOUNCE = 1,
			GRAB = 2
		};
	
		explicit MapObject(Point position, ClientMapObject* clientpart = NULL);
		virtual ~MapObject();

		virtual Type get_type() const { return MAP_OBJECT; }

		Point get_position() const { return m_position; }
		bool get_is_tiled() const { return m_is_tiled; }
		Vector get_tile_dimensions() const { return m_tile_dimensions; }
		float get_scale_x() const { return m_scale_x; }
		float get_scale_y() const { return m_scale_y; }
		float get_rotation() const { return m_rotation; }
		
		// Returns the team that owns this object, or 0 if none.
		virtual char get_team() const { return 0; }

		void set_position(Point position);
		void set_is_tiled(bool is_tiled);
		void set_tile_dimensions(Vector tile_dimensions);
		void set_scale_x(float scale_x);
		void set_scale_y(float scale_y);
		void set_rotation(float rotation);

		ClientMapObject* get_client_part();

		b2Shape* make_bounding_shape(const std::string& shape_string, Point position);
		
		// Initializes the physics for this object.
		virtual void initialize_physics(b2World* world) = 0;
		
		// Returns the shape that bounds this map object
		//  May return NULL if this map object has no bounding shape (i.e. it's decoration)
		virtual const b2Shape* get_bounding_shape () const = 0;

		// Returns true if the object has a bounding shape, false otherwise
		bool is_intersectable() const { return get_bounding_shape() != NULL; }
		
		// Returns true if the player can jump off of this obstacle, false otherwise
		virtual bool is_jumpable() const = 0;

		// Returns true if shots can hit this obstacle, false if they travel through the obstacle
		virtual bool is_shootable() const = 0;

		// Returns true if players can collide with this obstacle, false otherwise
		virtual bool is_collidable() const = 0;

		// Returns true if players can interact with this obstacle while within its bounds, false otherwise
		virtual bool is_interactive() const = 0;


		// Returns true if the player is currently engaging this obstacle
		virtual bool is_engaged() const = 0;


		// Called when this obstacle was shot by the given player, at the given point.
		//  (Only called if is_shootable() returns true)
		//  Returns true if the obstacle took the hit, false if the shot was redirected.
		virtual bool shot(Player* shooter, Point point_hit, float direction) = 0;

		// Called when an object starts colliding with the obstacle
		//  contact is the Box2D contact manifold
		virtual CollisionResult collide(PhysicsObject* other, b2Contact* contact) = 0;

		// Called every frame during which an object is _within_ this obstacle's bounds
		//  (Only called if is_interactive() returns true)
		//  is_engaged() should return true after this function returns
		virtual void interact(PhysicsObject* other) = 0;

		// Called the first frame that the object is no longer within the obstacle's bounds
		//  (as determined by the result of calling is_engaged above)
		//  (Only called if is_interactive() returns true)
		//  is_engaged() should returns false after this function returns
		virtual void disengage(PhysicsObject* other) = 0;

		virtual void init(MapReader* reader);
	};
}

#include "Obstacle.hpp"
#include "Decoration.hpp"
#include "Gate.hpp"

#endif
