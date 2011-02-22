/*
 * ai/AI.cpp
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

#include "AI.hpp"
#include "common/GameLogic.hpp"
#include "common/Player.hpp"
#include "ai/StateTranslator.hpp"
#include "common/team.hpp"
#include "common/Weapon.hpp"
#include "common/PhysicsObject.hpp"

using namespace LM;
using namespace std;

AI::AI(GameLogic* logic) {
	m_logic = logic;
}

AI::~AI() {

}

void AI::set_own_player(Player* player) {
	m_player = player;
}

Player* AI::get_own_player() {
	return m_player;
}
		
void AI::set_other_player(Player* other_player) {
	m_other_player = other_player;
}

Player* AI::get_other_player() {
	return m_other_player;
}

const list<pair<const char*, float> >& AI::get_vars() {
	m_varlist.clear();
	
	Gate* enemy_gate = m_logic->get_map()->get_gate(get_other_team(m_player->get_team()));
	Gate* allied_gate = m_logic->get_map()->get_gate(m_player->get_team());
	Gate* other_enemy_gate = m_logic->get_map()->get_gate(get_other_team(m_other_player->get_team()));
	Gate* other_allied_gate = m_logic->get_map()->get_gate(m_other_player->get_team());
	
	m_varlist.push_back(make_pair("dist_to_other", dist_between_players(m_player, m_other_player)));
	m_varlist.push_back(make_pair("dist_to_my_gate", dist_to_own_gate(m_player)));
	m_varlist.push_back(make_pair("dist_to_enemy_gate", dist_to_enemy_gate(m_player)));
	m_varlist.push_back(make_pair("other_dist_to_enemy_gate", dist_to_enemy_gate(m_other_player)));
	m_varlist.push_back(make_pair("other_dist_to_own_gate", dist_to_own_gate(m_other_player)));
	m_varlist.push_back(make_pair("holding_gate", holding_gate(m_player)));
	m_varlist.push_back(make_pair("other_holding_gate", holding_gate(m_other_player)));
	m_varlist.push_back(make_pair("my_energy_percent", energy_percent(m_player)));
	m_varlist.push_back(make_pair("other_energy_percent", energy_percent(m_other_player)));
	m_varlist.push_back(make_pair("gun_cooldown", gun_cooldown(m_player)));
	m_varlist.push_back(make_pair("gun_angle_to_other", gun_angle_to_player(m_player, m_other_player)));
	m_varlist.push_back(make_pair("time_to_impact", time_to_impact(m_player)));
	m_varlist.push_back(make_pair("other_time_to_impact", time_to_impact(m_other_player)));
	m_varlist.push_back(make_pair("can_see_other", can_see_player(m_player, m_other_player)));
	m_varlist.push_back(make_pair("can_see_enemy_gate", can_see_gate(m_player, enemy_gate)));
	m_varlist.push_back(make_pair("can_see_my_gate", can_see_gate(m_player, allied_gate)));
	m_varlist.push_back(make_pair("other_can_see_enemy_gate", can_see_gate(m_player, other_enemy_gate)));
	m_varlist.push_back(make_pair("other_can_see_own_gate", can_see_gate(m_player, other_allied_gate)));
	
	return m_varlist;
}

float AI::get_fuzzy_input_value(StateTranslator* translator, const std::string& subsection) {
	return translator->get_value(subsection, get_vars());
}

// Utility methods for get_vars():

float AI::dist_between_players(Player* first, Player* second) const {
	return (first->get_position() - second->get_position()).get_magnitude();
}

float AI::dist_to_own_gate(Player* player) const {
	Gate* my_gate = m_logic->get_map()->get_gate(player->get_team());
	return (player->get_position() - my_gate->get_position()).get_magnitude();
}

float AI::dist_to_enemy_gate(Player* player) const {
	Gate* enemy_gate = m_logic->get_map()->get_gate(get_other_team(player->get_team()));
	return (player->get_position() - enemy_gate->get_position()).get_magnitude();
}

float AI::holding_gate(Player* player) const {
	Gate* enemy_gate = m_logic->get_map()->get_gate(get_other_team(player->get_team()));
	return m_logic->is_engaging_gate(player->get_id(), get_other_team(player->get_team())) ? enemy_gate->get_progress() : 0;
}

float AI::energy_percent(Player* player) const {
	return (((float)player->get_energy()) / Player::MAX_ENERGY);
}

float AI::gun_cooldown(Player* player) const {
	Weapon* weapon = m_logic->get_weapon(player->get_current_weapon_id());
	
	if (weapon == NULL) {
		return 0;
	}
	
	return weapon->get_remaining_cooldown();
}

float AI::gun_angle_to_player(Player* player, Player* other) const {
	float x_dist = other->get_x() - player->get_x();
	float y_dist = other->get_y() - player->get_y();
	float wanted_angle = atan2(y_dist, x_dist);
	return fabs(wanted_angle - player->get_gun_rotation_radians());
}

float AI::time_to_impact(Player* player) {
	if (grabbing_wall(player)) {
		return 0;
	}

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
	
	return to_game(found_distance) / player->get_velocity().get_magnitude();
}

float AI::can_see_player(Player* player, Player* other_player, float max_radius) {
	b2Vec2 ray_start = b2Vec2(to_physics(player->get_x()), to_physics(player->get_y()));
	b2Vec2 target_pos = b2Vec2(to_physics(other_player->get_x()), to_physics(other_player->get_y()));
	float x_end = target_pos.x - ray_start.x;
	float y_end = target_pos.x - ray_start.y;
	float wanted_angle = atan2(y_end, x_end);
	
	// Perform the first raycast, at the center of the player.
	do_ray_cast(ray_start, wanted_angle, max_radius, player);

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
			if (hitobj->get_type() == PhysicsObject::PLAYER && to_game(m_ray_cast.shortest_dist) < max_radius) {
				Player* hitplayer = static_cast<Player*>(hitobj);
				if (hitplayer->get_id() == other_player->get_id()) {
					break;
				}
			}
			
			b2Vec2 vertex = polyshape->GetVertex(index);
		
			float x_end = to_physics(other_player->get_x()) + vertex.x * cos(other_player->get_rotation_radians());
			float y_end = to_physics(other_player->get_y()) + vertex.y * sin(other_player->get_rotation_radians());
			float wanted_angle = atan2(y_end, x_end);
			
			do_ray_cast(ray_start, wanted_angle, max_radius, player);
			index++;
		}
	}
	
	PhysicsObject* hitobj = m_ray_cast.closest_object;
	if (hitobj->get_type() != PhysicsObject::PLAYER || to_game(m_ray_cast.shortest_dist) > max_radius) {
		return numeric_limits<float>::max();
	}
	
	Player* hitplayer = static_cast<Player*>(hitobj);
	if (hitplayer->get_id() != other_player->get_id()) {
		return numeric_limits<float>::max();
	}
	
	return m_ray_cast.shortest_dist;
}

float AI::can_see_gate(Player* player, Gate* gate, float max_radius) {
	b2Vec2 ray_start = b2Vec2(to_physics(player->get_x()), to_physics(player->get_y()));
	Point gate_pos = gate->get_position();
	b2Vec2 target_pos = b2Vec2(to_physics(gate_pos.x), to_physics(gate_pos.y));
	float x_end = target_pos.x - ray_start.x;
	float y_end = target_pos.x - ray_start.y;
	float wanted_angle = atan2(y_end, x_end);
	
	// Perform the first raycast, at the center of the gate
	do_ray_cast(ray_start, wanted_angle, max_radius, player);
	
	// Cast at the other corners
	const b2Shape* shape = gate->get_bounding_shape();
	if (shape->GetType() == b2Shape::e_polygon) {
		const b2PolygonShape* polyshape = static_cast<const b2PolygonShape*>(shape);
		int index = 0;
		while (index < polyshape->GetVertexCount()) {
			// Check if we've seen the gate
			PhysicsObject* hitobj = m_ray_cast.closest_object;
			if (hitobj->get_type() == PhysicsObject::MAP_OBJECT && to_game(m_ray_cast.shortest_dist) < max_radius) {
				if (hitobj == gate) {
					break;
				}
			}
			
			b2Vec2 vertex = polyshape->GetVertex(index);
		
			float x_end = to_physics(gate_pos.x) + vertex.x * cos(to_radians(gate->get_rotation()));
			float y_end = to_physics(gate_pos.y) + vertex.y * sin(to_radians(gate->get_rotation()));
			float wanted_angle = atan2(y_end, x_end);
			
			do_ray_cast(ray_start, wanted_angle, max_radius, player);
			index++;
		}
	}
	
	PhysicsObject* hitobj = m_ray_cast.closest_object;
	if (hitobj->get_type() != PhysicsObject::MAP_OBJECT || to_game(m_ray_cast.shortest_dist) > max_radius || hitobj != gate) {
		return numeric_limits<float>::max();
	}
	
	return to_game(m_ray_cast.shortest_dist);
}

float AI::do_ray_cast(b2Vec2& start_point, float direction, float distance = -1, PhysicsObject* starting_object = NULL) {
	if (distance == -1) {
		distance = 20000;
	}
	float end_x = start_point.x + cos(direction) * distance;
	float end_y = start_point.y + sin(direction) * distance;
	
	m_ray_cast.ray_start = start_point;
	m_ray_cast.ray_end = b2Vec2(end_x, end_y);
	m_ray_cast.ray_direction = direction;
	m_ray_cast.start_object = starting_object;
	
	b2World* world = m_logic->get_world();
	if (world == NULL) {
		return -1;
	}
	
	world->RayCast(this, start_point, m_ray_cast.ray_end);
	
	return m_ray_cast.shortest_dist;
}

// Boolean getters that could be useful

bool AI::is_active(Player* player) const {
	return !(player->is_frozen() || player->is_invisible());
}

bool AI::grabbing_wall(Player* player) const {
	return player->is_grabbing_obstacle();
}

float32 AI::ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float32 fraction) {
	b2Body* body = fixture->GetBody();
	
	if (body->GetUserData() == NULL) {
		WARN("Body has no user data!");
		return 1;
	}
	
	if (fraction < 0) {
		return 0;
	}
	
	PhysicsObject* hitobj = static_cast<PhysicsObject*>(body->GetUserData());
	
	if (fixture->IsSensor()) {
		return 1;
	}
	
	Point end = Point(point.x, point.y);
	float dist = (end-Point(m_ray_cast.ray_start.x, m_ray_cast.ray_start.y)).get_magnitude();
	
	if (m_ray_cast.shortest_dist != -1 && dist > m_ray_cast.shortest_dist) {
		return 1;
	}
	
	if (hitobj->get_type() == PhysicsObject::MAP_OBJECT) {
		MapObject* object = static_cast<MapObject*>(hitobj);
		
		if (!object->is_collidable()) {
			return 1;
		}
	}
	
	m_ray_cast.shortest_dist = dist;
	
	m_ray_cast.closest_object = hitobj;
	
	m_ray_cast.hit_point = b2Vec2(end.x, end.y);
	
	return 1;
}
