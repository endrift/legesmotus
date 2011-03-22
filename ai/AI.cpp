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
#include "common/team.hpp"
#include "common/Weapon.hpp"
#include "common/PhysicsObject.hpp"
#include "common/RayCast.hpp"
#include "SparseIntersectMap.hpp"
#include "Pathfinder.hpp"

using namespace LM;
using namespace std;

AI::AI(const GameLogic* logic) {
	m_logic = logic;
	m_player = NULL;
	m_other_player = NULL;
}

AI::~AI() {
}

void AI::set_logic(const GameLogic* logic) {
	m_logic = logic;
}

const GameLogic* AI::get_logic() const {
	return m_logic;
}

void AI::set_own_player(const Player* player) {
	m_player = player;
}

const Player* AI::get_own_player() const {
	return m_player;
}
		
void AI::set_other_player(const Player* other_player) {
	m_other_player = other_player;
}

const Player* AI::get_other_player() const {
	return m_other_player;
}

void AI::randomize_aim_inaccuracy() {
	// Do nothing.
}

void AI::update(const GameLogic& logic, uint64_t diff) {
	set_logic(&logic);

	// Deal with MapGrapher if necessary:
	// If we don't know anything about the map, start learning about it.
	if (get_map_graph() == NULL) {
		initialize_map_grapher();
	}

	// If the MapGrapher is not done, continue it:
	if (!m_grapher.is_done_mapping()) {
		m_grapher.do_mapping(1);

		// We're done now, let's output the results
		if (m_grapher.is_done_mapping()) {
			m_grapher.write_map(logic.get_map()->get_name());
		}
	}
	
	step(logic, diff);
}

void AI::step(const GameLogic& logic, uint64_t diff) {
	// Do nothing.
}

float AI::find_desired_aim() {
	return 0;
}

AI::AimReason AI::get_aim_reason() {
	return DO_NOTHING;
}

// Utility methods for get_vars():

float AI::dist_between_players(const Player* first, const Player* second) const {
	return to_physics((first->get_position() - second->get_position()).get_magnitude());
}

float AI::dist_to_own_gate(const Player* player) const {
	const Gate* my_gate = m_logic->get_map()->get_gate(player->get_team());
	return to_physics((player->get_position() - my_gate->get_position()).get_magnitude());
}

float AI::dist_to_enemy_gate(const Player* player) const {
	const Gate* enemy_gate = m_logic->get_map()->get_gate(get_other_team(player->get_team()));
	return to_physics((player->get_position() - enemy_gate->get_position()).get_magnitude());
}

float AI::holding_gate(const Player* player) const {
	const Gate* enemy_gate = m_logic->get_map()->get_gate(get_other_team(player->get_team()));
	return m_logic->is_engaging_gate(player->get_id(), get_other_team(player->get_team())) ? enemy_gate->get_progress() : 0;
}

float AI::energy_percent(const Player* player) const {
	return (((float)player->get_energy()) / Player::MAX_ENERGY);
}

float AI::gun_cooldown(const Player* player) const {
	const Weapon* weapon = m_logic->get_weapon(player->get_current_weapon_id());
	
	if (weapon == NULL) {
		return 0;
	}
	
	return weapon->get_remaining_cooldown();
}

float AI::gun_angle_to_player(const Player* player, const Player* other) const {
	float x_dist = other->get_x() - player->get_x();
	float y_dist = other->get_y() - player->get_y();
	float wanted_angle = atan2(y_dist, x_dist);
	float curr_aim = get_normalized_angle(player->get_gun_rotation_radians() + player->get_rotation_radians());
	int dir = curr_aim > wanted_angle ? -1 : 1;
	if (fabs(wanted_angle - curr_aim) > M_PI) {
		curr_aim += 2 * M_PI * dir;
	}
	return fabs(wanted_angle - curr_aim);
}

float AI::time_to_impact(const Player* player) const {
	const b2World* world = m_logic->get_world();

	if (world == NULL || grabbing_wall(player)) {
		return 0;
	}

	RayCast cast(world);
	float found_distance = cast.cast_in_vel_dir(player);
	
	return to_game(found_distance) / player->get_velocity().get_magnitude();
}

float AI::can_see_player(const Player* player, const Player* other_player, float max_radius) {
	const b2World* world = m_logic->get_world();

	if (world == NULL) {
		return 0;
	}

	RayCast cast(world);
	b2Vec2 start_pos = b2Vec2(to_physics(player->get_x()), to_physics(player->get_y()));
	float dist = cast.cast_at_player(start_pos, other_player, max_radius);
	return dist;
}

float AI::can_see_gate(const Player* player, const Gate* gate, float max_radius) {
	b2Vec2 ray_start = b2Vec2(to_physics(player->get_x()), to_physics(player->get_y()));
	
	const b2World* world = m_logic->get_world();

	if (world == NULL) {
		return 0;
	}
	
	RayCast cast(world);
	float dist = cast.cast_at_obstacle(ray_start, gate, max_radius, true);
	
	return dist;
}

// Boolean getters that could be useful

bool AI::is_active(const Player* player) const {
	return !(player->is_frozen() || player->is_invisible());
}

bool AI::grabbing_wall(const Player* player) const {
	return player->is_grabbing_obstacle();
}

SparseIntersectMap* AI::get_map_graph() {
	return m_grapher.get_graph();
}

void AI::initialize_map_grapher() {
	// NOTE: Const cast here is necessary because MapGrapher can only work
	// with a non-const b2World for now, due to b2World's GetBodyList() method.
	// This is fixed in Box2D's trunk, but not in any released version yet
	b2World* world = const_cast<b2World*>(m_logic->get_world());

	if (world == NULL) {
		return;
	}
	m_grapher.load_map(m_logic, world);
	
	m_pathfinder.set_graph(get_map_graph());
}

Pathfinder* AI::get_pathfinder() {
	return &m_pathfinder;
}

bool AI::find_path(const Player* my_player, float end_x, float end_y, float tolerance, std::vector<SparseIntersectMap::Intersect>& path, b2Vec2* start) {
	ASSERT(start != NULL);

	uint64_t pathfind_start_time = get_ticks();

	b2Body* body = my_player->get_physics_body();
	// XXX: Do we just want to use the first fixture?
	b2Fixture* fixture = &body->GetFixtureList()[0];
	b2Shape* shape = fixture->GetShape();
	bool path_found = false;
	if (shape->GetType() == b2Shape::e_polygon) {
		b2PolygonShape* polyshape = static_cast<b2PolygonShape*>(shape);
		int index = 0;
		while (index < polyshape->GetVertexCount()) {
			b2Vec2 vertex = polyshape->GetVertex(index);
			b2Vec2 world = body->GetWorldPoint(vertex);
			if (m_pathfinder.find_path(to_game(world.x), to_game(world.y), end_x, end_y, tolerance, path)) {
				path_found = true;
				start->x = to_game(world.x);
				start->y = to_game(world.y);
				break;
			}
			index++;
		}
	
		if (path_found) {
			DEBUG("Finding path: Success: " << path_found << " Took: " << (get_ticks() - pathfind_start_time) << " ms.");
		}
	}
	
	return path_found;
}
