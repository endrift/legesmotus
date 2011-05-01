/*
 * common/RayCast.hpp
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

#ifndef LM_COMMON_RAYCAST_HPP
#define LM_COMMON_RAYCAST_HPP

#include "common/physics.hpp"

namespace LM {
	class PhysicsObject;
	class Player;
	class MapObject;

	class RayCast : public b2RayCastCallback {
	public:
	
	struct RayCastResult {
		const PhysicsObject*	start_object;	// The object (if any) where this ray started
		b2Vec2		ray_start;	// The starting point of the ray cast
		float		ray_direction;	// The angle (in radians) at which the ray was cast
		b2Vec2		ray_end;	// The maximum point on the ray
		PhysicsObject* 	closest_object; // The closest object
		float		shortest_dist;	// The closest hit-point on that object
		b2Vec2		hit_point;	// The point where the ray hit
	};

	private:
		RayCastResult m_ray_cast;
		const b2World* m_physics;
		bool m_ignore_collidable;

	public:
		RayCast();
		RayCast(const b2World* physics);
		
		~RayCast();
		
		RayCastResult& get_result();
		
		void set_physics(const b2World* physics);
	
		float cast_at_player(const Point& ray_start, const Player* other_player, float max_radius = -1);
		float cast_at_player(const b2Vec2& ray_start, const Player* other_player, float max_radius = -1);
		
		float cast_at_obstacle(const Point& ray_start, const MapObject* object, float max_radius = -1, bool ignore_collidable = false);
		float cast_at_obstacle(const b2Vec2& ray_start, const MapObject* object, float max_radius = -1, bool ignore_collidable = false);
		
		float cast_in_vel_dir(const Player* player);
	
		float do_ray_cast(const b2Vec2& start_point, float direction, float distance = -1, const PhysicsObject* starting_object = NULL, bool ignore_collidable = false);
	
		// Box2D Physics Callbacks
		float32 ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float32 fraction);
	};
}

#endif
