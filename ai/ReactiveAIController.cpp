/*
 * ai/ReactiveAIController.cpp
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

#include "ReactiveAIController.hpp"
#include "common/PhysicsObject.hpp"
#include <cstdlib>
#include "common/team.hpp"

using namespace LM;
using namespace std;

const float ReactiveAIController::MAX_AIM_VEL = .04f;
const unsigned int ReactiveAIController::AIM_TOLERANCE = .01f;
const float ReactiveAIController::BASE_AIM_UNCERTAINTY = .2f;
const float ReactiveAIController::QUICK_AIM_CHANGE_ERROR = .1f;
const unsigned int ReactiveAIController::VISION_RADIUS = 700;
const unsigned int ReactiveAIController::GUN_ANGLE_CHANGE_RATE = 60;

ReactiveAIController::ReactiveAIController() {
	srand(time(NULL));

	m_changeset = 0;
	
	m_wanted_aim = 0;
	m_curr_aim = 0;
	m_enemy_gate = 0;
	m_aim_reason = JUMP;
}

void ReactiveAIController::find_desired_aim(const GameLogic& state, uint32_t player_id) {
	Player* closestenemy = NULL;
	uint32_t closestdist = std::numeric_limits<uint32_t>::max();
	
	const Player* my_player = state.get_player(player_id);
	if (my_player == NULL) {
		return;
	}
	
	// Find the nearest enemy.
	ConstIterator<std::pair<uint32_t, Player*> > it=state.list_players();
	while(it.has_more()) {
		std::pair<uint32_t, Player*> next = it.next();
		if (next.first == player_id) {
			continue;
		}
		
		Player* currplayer = next.second;
		
		if (currplayer->get_team() == my_player->get_team()) {
			continue;
		}
		
		if (currplayer->is_frozen() || currplayer->is_invisible()) {
			continue;
		}
		
		float dist = check_player_visible(state.get_world(), my_player, currplayer);
		if (dist > -1) {
			closestenemy = currplayer;
			closestdist = dist;
		}
	}
	
	// Check if we can see the enemy's gate:
	m_enemy_gate = state.get_map()->get_gate(get_other_team(my_player->get_team()));
	
	float dist = -1;
	if (m_enemy_gate != NULL) {
		dist = check_gate_visible(state.get_world(), my_player, m_enemy_gate);
	}
	
	if (dist > -1 && dist < VISION_RADIUS && 
			!state.is_engaging_gate(my_player->get_id(), get_other_team(my_player->get_team())) && 
			my_player->is_grabbing_obstacle()) {
		Point gate_pos = m_enemy_gate->get_position();
		float x_dist = gate_pos.x - my_player->get_x();
		float y_dist = gate_pos.y - my_player->get_y();
		m_wanted_aim = atan2(y_dist, x_dist);
		m_aim_reason = JUMP;
	} else if (closestdist < VISION_RADIUS) {
		// Find the angle to turn towards the enemy.
		float x_dist = closestenemy->get_x() - my_player->get_x();
		float y_dist = closestenemy->get_y() - my_player->get_y();
		m_wanted_aim = atan2(y_dist, x_dist) + m_aim_inaccuracy;
		m_aim_reason = FIRE;
		
		if (rand()%(GUN_ANGLE_CHANGE_RATE/2) == 1) {
			float aimdiff = fabs(m_curr_aim - m_wanted_aim);
		
			// Change our aim error.
			float uncertainty = BASE_AIM_UNCERTAINTY + (aimdiff * QUICK_AIM_CHANGE_ERROR);
			m_aim_inaccuracy = (float)rand()/(float)RAND_MAX * uncertainty * 2 - uncertainty;
		}
	} else {
		// For now, just randomly aim somewhere.
		// XXX: Can we lock this to framerate better?
		if (rand()%GUN_ANGLE_CHANGE_RATE == 1) {
			// 1/5th chance to jump towards the gate, even if we can't see it.
			if (rand() % 5 == 1) {
				Point gate_pos = m_enemy_gate->get_position();
				float x_dist = gate_pos.x - my_player->get_x();
				float y_dist = gate_pos.y - my_player->get_y();
				m_wanted_aim = atan2(y_dist, x_dist);
				m_aim_reason = JUMP;
			} else {
				m_wanted_aim = to_radians(rand()%360 - 180);
				m_aim_reason = JUMP;
			}
		}
	}
}

float ReactiveAIController::update_gun() {
	float aimdiff = fabs(m_curr_aim - m_wanted_aim);
	
	if (aimdiff > AIM_TOLERANCE) {
		m_changes[m_changeset ^ 1] |= CHANGE_AIM;
		int dir = m_curr_aim > m_wanted_aim ? -1 : 1;
		
		// Avoid going the long way around.
		if (aimdiff > M_PI) {
			m_curr_aim += 2 * M_PI * dir;
			dir *= -1;
		}
		
		m_curr_aim += dir * min(MAX_AIM_VEL, dir * (m_wanted_aim - m_curr_aim));
	}
	
	return fabs(m_curr_aim - m_wanted_aim);
}

float ReactiveAIController::check_gate_visible(const b2World* physics, const Player* start_player, const Gate* gate) {
	// XXX: Question - is there a way to avoid all this repeated code.

	m_ray_start = Point(to_physics(start_player->get_x()), to_physics(start_player->get_y()));

	m_ray_gate_team = 0;
	m_ray_shortest_dist = -1;
	
	Point gate_pos = gate->get_position();
	
	// Perform the first raycast, at the center of the gate.
	physics->RayCast(this, b2Vec2(m_ray_start.x, m_ray_start.y), b2Vec2(to_physics(gate_pos.x), to_physics(gate_pos.y)));
	
	const b2Shape* shape = gate->get_bounding_shape();
	if (shape->GetType() == b2Shape::e_polygon) {
		const b2PolygonShape* polyshape = static_cast<const b2PolygonShape*>(shape);
		int index = 0;
		while (m_ray_gate_team != gate->get_team() && index < polyshape->GetVertexCount()) {
			b2Vec2 vertex = polyshape->GetVertex(index);
		
			float x = to_physics(gate_pos.x) + vertex.x * cos(to_radians(gate->get_rotation()));
			float y = to_physics(gate_pos.y) + vertex.y * sin(to_radians(gate->get_rotation()));
			
			m_ray_shortest_dist = -1;
			physics->RayCast(this, b2Vec2(m_ray_start.x, m_ray_start.y), b2Vec2(x, y));
			if (m_ray_gate_team == gate->get_team()) {
				break;
			}
			index++;
		}
	}
	
	float shortest_dist = to_game(m_ray_shortest_dist);
	
	if (m_ray_gate_team != gate->get_team() || shortest_dist > VISION_RADIUS) {
		return -1;
	}
	
	return shortest_dist;
}

float ReactiveAIController::check_player_visible(const b2World* physics, const Player* start_player, const Player* other_player) {
	// XXX: Question - is there a way to avoid all this repeated code.	
	
	m_ray_start = Point(to_physics(start_player->get_x()), to_physics(start_player->get_y()));

	m_ray_hit_player = -1;
	m_ray_shortest_dist = -1;
	
	// Perform the first raycast, at the center of the player.
	physics->RayCast(this, b2Vec2(m_ray_start.x, m_ray_start.y), b2Vec2(to_physics(other_player->get_x()), to_physics(other_player->get_y())));

	b2Body* body = other_player->get_physics_body();
	// XXX: Do we just want to use the first fixture?
	b2Fixture* fixture = &body->GetFixtureList()[0];
	b2Shape* shape = fixture->GetShape();
	if (shape->GetType() == b2Shape::e_polygon) {
		b2PolygonShape* polyshape = static_cast<b2PolygonShape*>(shape);
		int index = 0;
		while (m_ray_hit_player != other_player->get_id() && index < polyshape->GetVertexCount()) {
			b2Vec2 vertex = polyshape->GetVertex(index);
		
			float x = to_physics(other_player->get_x()) + vertex.x * cos(other_player->get_rotation_radians());
			float y = to_physics(other_player->get_y()) + vertex.y * sin(other_player->get_rotation_radians());
			
			m_ray_shortest_dist = -1;
			physics->RayCast(this, b2Vec2(m_ray_start.x, m_ray_start.y), b2Vec2(x, y));
			if (m_ray_hit_player == other_player->get_id()) {
				break;
			}
			index++;
		}
	}
	
	float shortest_dist = to_game(m_ray_shortest_dist);
	
	if (m_ray_hit_player != other_player->get_id() || shortest_dist > VISION_RADIUS) {
		return -1;
	}
	
	return shortest_dist;
}

void ReactiveAIController::update(uint64_t diff, const GameLogic& state, int player_id) {
	m_changes[m_changeset] = NO_CHANGE;
	m_changeset ^= 1;
	
	const Player* my_player = state.get_player(player_id);
	if (my_player == NULL) {
		return;
	}
	
	if (my_player->is_frozen()) {
		return;
	}
	
	// Determine desired aim.
	find_desired_aim(state, player_id);
	
	// Turn gun towards wanted aim.
	float aimdiff = update_gun();
	
	// XXX: For now, we fire even if we're aiming to jump instead of to attack a player.
	if (aimdiff <= AIM_TOLERANCE) {
		if (m_aim_reason == FIRE) {
			m_changes[m_changeset ^ 1] |= FIRE_WEAPON;
			m_changes[m_changeset ^ 1] |= STOP_JUMPING;
		} else {
			// Determine whether to jump.
			if (state.is_engaging_gate(player_id, get_other_team(my_player->get_team()))) {
				m_changes[m_changeset ^ 1] |= STOP_JUMPING;
			} else {
				m_changes[m_changeset ^ 1] |= JUMPING;
			}
		}
	} else {
		m_changes[m_changeset ^ 1] |= STOP_JUMPING;
	}
}

int ReactiveAIController::get_changes() const {
	return m_changes[m_changeset];
}

float ReactiveAIController::get_aim() const {
	return m_curr_aim;
}

float ReactiveAIController::get_distance() const {
	return 0;
}

int ReactiveAIController::get_weapon() const {
	return 0;
}

wstring ReactiveAIController::get_message() const {
	return NULL;
}

bool ReactiveAIController::message_is_team_only() const {
	return false;
}

void ReactiveAIController::received_message(const Player* p, const wstring& message) {
}

float32 ReactiveAIController::ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float32 fraction) {
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
	float dist = (end-m_ray_start).get_magnitude();
	
	if (m_ray_shortest_dist != -1 && dist > m_ray_shortest_dist) {
		return 1;
	}
	m_ray_shortest_dist = dist;
	
	if (hitobj->get_type() == PhysicsObject::MAP_OBJECT) {
		MapObject* object = static_cast<MapObject*>(hitobj);
		
		if (hitobj == m_enemy_gate) {
			m_ray_gate_team = m_enemy_gate->get_team();
		} else {
			m_ray_gate_team = 0;
		}
		
		if (!object->is_collidable()) {
			return 1;
		}
	} else {
		m_ray_gate_team = 0;
	}
	
	if (hitobj->get_type() == PhysicsObject::PLAYER) {
		Player* hitplayer = static_cast<Player*>(hitobj);
		m_ray_hit_player = hitplayer->get_id();
	} else {
		m_ray_hit_player = -1;
	}
	
	return 1;
}
