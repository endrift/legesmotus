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
#include "common/RayCast.hpp"

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

const GameLogic* AI::get_logic() {
	return m_logic;
}

void AI::set_own_player(const Player* player) {
	m_player = player;
}

const Player* AI::get_own_player() {
	return m_player;
}
		
void AI::set_other_player(const Player* other_player) {
	m_other_player = other_player;
}

const Player* AI::get_other_player() {
	return m_other_player;
}

void AI::update(const GameLogic& logic, uint64_t diff) {
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
	return (first->get_position() - second->get_position()).get_magnitude();
}

float AI::dist_to_own_gate(const Player* player) const {
	const Gate* my_gate = m_logic->get_map()->get_gate(player->get_team());
	return (player->get_position() - my_gate->get_position()).get_magnitude();
}

float AI::dist_to_enemy_gate(const Player* player) const {
	const Gate* enemy_gate = m_logic->get_map()->get_gate(get_other_team(player->get_team()));
	return (player->get_position() - enemy_gate->get_position()).get_magnitude();
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
	return fabs(wanted_angle - player->get_gun_rotation_radians());
}

float AI::time_to_impact(const Player* player) {
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
