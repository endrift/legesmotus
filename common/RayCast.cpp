/*
 * common/RayCast.cpp
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

#include "RayCast.hpp"
#include "common/misc.hpp"
#include "common/PhysicsObject.hpp"
#include "common/MapObject.hpp"
#include "common/Player.hpp"

using namespace LM;
using namespace std;


RayCast::RayCast() {
	m_physics = NULL;
	m_ignore_collidable = false;
}

RayCast::RayCast(const b2World* physics) {
	m_physics = physics;
	m_ignore_collidable = false;
}

RayCast::~RayCast() {
}

RayCast::RayCastResult& RayCast::get_result() {
	return m_ray_cast;
}

void RayCast::set_physics(const b2World* physics) {
	m_physics = physics;
}

float RayCast::do_ray_cast(const b2Vec2& start_point, float direction, float distance = -1, const PhysicsObject* starting_object, bool ignore_collidable) {
	m_ignore_collidable = ignore_collidable;
	
	if (distance == -1) {
		distance = 20000;
	}
	float end_x = start_point.x + cos(direction) * distance;
	float end_y = start_point.y + sin(direction) * distance;
	
	m_ray_cast.ray_start = start_point;
	m_ray_cast.ray_end = b2Vec2(end_x, end_y);
	m_ray_cast.ray_direction = direction;
	m_ray_cast.start_object = starting_object;
	m_ray_cast.closest_object = NULL;
	m_ray_cast.shortest_dist = -1;
	m_ray_cast.hit_point = b2Vec2(-1, -1);
	
	if (m_physics == NULL) {
		return -1;
	}
	
	m_physics->RayCast(this, start_point, m_ray_cast.ray_end);
	
	return m_ray_cast.shortest_dist;
}

float RayCast::cast_at_player(const Point& ray_start, const Player* other_player, float max_radius) {
	b2Vec2 start = b2Vec2(ray_start.x, ray_start.y);

	return cast_at_player(start, other_player, max_radius);
}

float RayCast::cast_at_player(const b2Vec2& ray_start, const Player* other_player, float max_radius) {
	b2Vec2 target_pos = b2Vec2(to_physics(other_player->get_x()), to_physics(other_player->get_y()));
	float x_end = target_pos.x - ray_start.x;
	float y_end = target_pos.y - ray_start.y;
	float wanted_angle = atan2(y_end, x_end);
	
	// Perform the first raycast, at the center of the player.
	do_ray_cast(ray_start, wanted_angle, max_radius);

	b2Body* body = other_player->get_physics_body();
	// XXX: Do we just want to use the first fixture?
	b2Fixture* fixture = &body->GetFixtureList()[0];
	b2Shape* shape = fixture->GetShape();
	if (shape->GetType() == b2Shape::e_polygon) {
		b2PolygonShape* polyshape = static_cast<b2PolygonShape*>(shape);
		int index = 0;
		while (index < polyshape->GetVertexCount()) {
			// Check if we've seen the player
			PhysicsObject* hitobj = m_ray_cast.closest_object;
			if (hitobj != NULL) {
				if (hitobj->get_type() == PhysicsObject::PLAYER && (max_radius == -1 || to_game(m_ray_cast.shortest_dist) < max_radius)) {
					Player* hitplayer = static_cast<Player*>(hitobj);
					if (hitplayer->get_id() == other_player->get_id()) {
						break;
					}
				}
			}
			
			b2Vec2 vertex = polyshape->GetVertex(index);
		
			float x_end = to_physics(other_player->get_x()) + vertex.x * cos(other_player->get_rotation_radians());
			float y_end = to_physics(other_player->get_y()) + vertex.y * sin(other_player->get_rotation_radians());
			float wanted_angle = atan2(y_end, x_end);
			
			do_ray_cast(ray_start, wanted_angle, max_radius);
			index++;
		}
	}
	
	PhysicsObject* hitobj = m_ray_cast.closest_object;
	if (hitobj == NULL) {
		return numeric_limits<float>::max();
	}
	
	if (hitobj->get_type() != PhysicsObject::PLAYER || (max_radius != -1 && to_game(m_ray_cast.shortest_dist) > max_radius)) {
		return numeric_limits<float>::max();
	}
	
	Player* hitplayer = static_cast<Player*>(hitobj);
	if (hitplayer->get_id() != other_player->get_id()) {
		return numeric_limits<float>::max();
	}
	
	return m_ray_cast.shortest_dist;
}

float RayCast::cast_at_obstacle(const Point& ray_start, const MapObject* object, float max_radius, bool ignore_collidable) {
	b2Vec2 start = b2Vec2(ray_start.x, ray_start.y);

	return cast_at_obstacle(start, object, max_radius, ignore_collidable);
}

float RayCast::cast_at_obstacle(const b2Vec2& ray_start, const MapObject* object, float max_radius, bool ignore_collidable) {
	Point object_pos = object->get_position();
	b2Vec2 target_pos = b2Vec2(to_physics(object_pos.x), to_physics(object_pos.y));
	float x_end = target_pos.x - ray_start.x;
	float y_end = target_pos.y - ray_start.y;
	float wanted_angle = atan2(y_end, x_end);
	
	// Perform the first raycast, at the center of the object
	do_ray_cast(ray_start, wanted_angle, max_radius, NULL, ignore_collidable);
	
	// Cast at the other corners
	const b2Shape* shape = object->get_bounding_shape();
	if (shape->GetType() == b2Shape::e_polygon) {
		const b2PolygonShape* polyshape = static_cast<const b2PolygonShape*>(shape);
		int index = 0;
		while (index < polyshape->GetVertexCount()) {
			// Check if we've seen the object
			PhysicsObject* hitobj = m_ray_cast.closest_object;
			if (hitobj != NULL) {
				if (hitobj->get_type() == PhysicsObject::MAP_OBJECT && (max_radius == -1 || to_game(m_ray_cast.shortest_dist) < max_radius)) {
					if (hitobj == object) {
						break;
					}
				}
			}
			
			b2Vec2 vertex = polyshape->GetVertex(index);
		
			float x_end = to_physics(object_pos.x) + vertex.x * cos(to_radians(object->get_rotation()));
			float y_end = to_physics(object_pos.y) + vertex.y * sin(to_radians(object->get_rotation()));
			float wanted_angle = atan2(y_end, x_end);
			
			do_ray_cast(ray_start, wanted_angle, max_radius, NULL, ignore_collidable);
			index++;
		}
	}
	
	PhysicsObject* hitobj = m_ray_cast.closest_object;
	if (hitobj == NULL) {
		return numeric_limits<float>::max();
	}
	
	if (hitobj->get_type() != PhysicsObject::MAP_OBJECT || (max_radius != -1 && to_game(m_ray_cast.shortest_dist) > max_radius) || hitobj != object) {
		return numeric_limits<float>::max();
	}
	
	return m_ray_cast.shortest_dist;
}

float RayCast::cast_in_vel_dir(const Player* player) {
	b2Vec2 ray_start = b2Vec2(to_physics(player->get_x()), to_physics(player->get_y()));
	float wanted_angle = atan2(player->get_y_vel(), player->get_x_vel());
	
	float found_distance = 0;
	
	// Perform the first raycast, from the center of the player.
	do_ray_cast(ray_start, wanted_angle, -1.0f, player);
	
	found_distance = m_ray_cast.shortest_dist;

	b2Body* body = player->get_physics_body();
	// XXX: Do we just want to use the first fixture?
	b2Fixture* fixture = &body->GetFixtureList()[0];
	b2Shape* shape = fixture->GetShape();
	if (shape->GetType() == b2Shape::e_polygon) {
		b2PolygonShape* polyshape = static_cast<b2PolygonShape*>(shape);
		int index = 0;
		while (index < polyshape->GetVertexCount()) {			
			b2Vec2 vertex = polyshape->GetVertex(index);
		
			float x_start = to_physics(player->get_x()) + vertex.x * cos(player->get_rotation_radians());
			float y_start = to_physics(player->get_y()) + vertex.y * sin(player->get_rotation_radians());
			b2Vec2 start = b2Vec2(x_start, y_start);
			
			do_ray_cast(start, wanted_angle, -1.0f, player);
			
			if (m_ray_cast.shortest_dist < found_distance) {
				found_distance = m_ray_cast.shortest_dist;
			}
			
			index++;
		}
	}
	
	return found_distance;
}

float32 RayCast::ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float32 fraction) {
	b2Body* body = fixture->GetBody();
	
	if (body->GetUserData() == NULL) {
		WARN("Body has no user data!");
		return 1;
	}
	
	if (fraction < 0) {
		return 1;
	}
	
	PhysicsObject* hitobj = static_cast<PhysicsObject*>(body->GetUserData());
	Point end = Point(point.x, point.y);
	float dist = (end-Point(m_ray_cast.ray_start.x, m_ray_cast.ray_start.y)).get_magnitude();
	
	if (fixture->IsSensor()) {
		return 1;
	}
	
	if (m_ray_cast.shortest_dist != -1 && dist > m_ray_cast.shortest_dist) {
		return 1;
	}
	
	if (hitobj->get_type() == PhysicsObject::MAP_OBJECT) {
		MapObject* object = static_cast<MapObject*>(hitobj);
		
		if (!m_ignore_collidable && !object->is_collidable()) {
			return 1;
		}
	}
	
	m_ray_cast.shortest_dist = dist;
	
	m_ray_cast.closest_object = hitobj;
	
	m_ray_cast.hit_point = b2Vec2(end.x, end.y);
	
	return 1;
}
