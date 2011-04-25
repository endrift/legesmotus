/*
 * ai/FuzzyLogicAI.cpp
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

#include "FuzzyLogicAI.hpp"
#include "common/team.hpp"
#include "common/Weapon.hpp"
#include "AggressiveState.hpp"
#include "DefensiveState.hpp"
#include "FuzzyLogicState.hpp"

using namespace LM;
using namespace std;

const float FuzzyLogicAI::AREA_AVOID_WEIGHT=500.0f;
const float FuzzyLogicAI::AREA_AVOID_SIZE=150.0f;
const uint64_t FuzzyLogicAI::ALLOWED_IDLE_TIME = 2000;
const uint64_t FuzzyLogicAI::MAX_WEAPON_SWITCH_FREQ = 5000;

FuzzyLogicAI::FuzzyLogicAI(const Configuration* config, const GameLogic* logic) : AI(logic) {
	srand(time(NULL));
	
	m_fuzzy = new FuzzyLogic("default");
	m_config = config;
	
	m_target = NULL;
	
	m_last_aim = 0.0f;
	
	// TODO: Load this from a config file
	m_max_aim_inaccuracy = 0.2f;
	
	m_aim_reason = DO_NOTHING;
	
	m_curr_weapon = 0;
	m_last_weapon_switch = 0;
	
	m_last_action = 0;
	
	m_found_path = false;
	
	initialize_logic();
	
	initialize_states();
}

FuzzyLogicAI::~FuzzyLogicAI() {
	delete m_fuzzy;
	delete m_fsm;
	
	m_states.clear();
}

void FuzzyLogicAI::initialize_logic() {
	// Load the categories.
	m_fuzzy->load_category(m_config, "dist_to_other");
	m_fuzzy->load_category(m_config, "dist_to_my_gate");
	m_fuzzy->load_category(m_config, "dist_to_enemy_gate");
	m_fuzzy->load_category(m_config, "other_dist_to_enemy_gate");
	m_fuzzy->load_category(m_config, "other_dist_to_own_gate");
	m_fuzzy->load_category(m_config, "holding_gate");
	m_fuzzy->load_category(m_config, "other_holding_gate");
	m_fuzzy->load_category(m_config, "my_energy_percent");
	m_fuzzy->load_category(m_config, "other_energy_percent");
	m_fuzzy->load_category(m_config, "gun_cooldown");
	m_fuzzy->load_category(m_config, "gun_angle_to_other");
	m_fuzzy->load_category(m_config, "time_to_impact");
	m_fuzzy->load_category(m_config, "other_time_to_impact");
	m_fuzzy->load_category(m_config, "can_see_player");
	m_fuzzy->load_category(m_config, "can_see_enemy_gate");
	m_fuzzy->load_category(m_config, "can_see_my_gate");
	m_fuzzy->load_category(m_config, "other_can_see_enemy_gate");
	m_fuzzy->load_category(m_config, "other_can_see_own_gate");
	m_fuzzy->load_category(m_config, "weap_damage_at_player");
	m_fuzzy->load_category(m_config, "weap_freeze_time");
	m_fuzzy->load_category(m_config, "weap_force");
}

void FuzzyLogicAI::initialize_states() {
	m_states["aggressive"] = new AggressiveState(m_fuzzy);
	m_states["defensive"] = new DefensiveState(m_fuzzy);
	
	int statenum = rand() % m_states.size();
	map<string, FuzzyLogicState*>::iterator it = m_states.begin();
	
	advance(it, statenum);

	m_fsm = new FuzzyLogicFSM((*it).first);
	DEBUG("AI starting with state: " << (*it).first);
	
	it = m_states.begin();
	for (; it != m_states.end(); it++) {
		m_fsm->set_state_data((*it).first, (*it).second);
	}
}

void FuzzyLogicAI::randomize_aim_inaccuracy() {
	float uncertainty = m_max_aim_inaccuracy;
	m_aim_inaccuracy = (float)rand()/(float)RAND_MAX * uncertainty * 2 - uncertainty;
}

void FuzzyLogicAI::step(const GameLogic& logic, uint64_t diff) {
	populate_environment();
	
	const Player* my_player = get_own_player();
	
	if (my_player == NULL) {
		return;
	}
	
	m_fuzzy->apply(&m_fuzzy_env);
	
	m_fsm->decide(this, &m_fuzzy_env, logic);
	
	// Check if we need to update our current gun.
	if (m_fsm->get_curr_weapon() != m_curr_weapon) {
		m_curr_weapon = m_fsm->get_curr_weapon();
		m_last_weapon_switch = get_ticks();
	}
	
	m_was_grabbing = my_player->is_grabbing_obstacle();
	m_last_aim = m_fsm->find_desired_aim();
}

bool FuzzyLogicAI::set_path(b2Vec2 start, vector<SparseIntersectMap::Intersect>& path) {
	m_found_path = true;

	if (path.size() == 0) {
		m_found_path = false;
		m_current_path.clear();
		return false;
	}
	m_current_path = path;
	return true;
}

void FuzzyLogicAI::set_found_path(bool found) {
	m_found_path = found;
}

float FuzzyLogicAI::get_next_aim(b2Vec2 start, vector<SparseIntersectMap::Intersect>& path) {
	SparseIntersectMap::Intersect jump_loc = path[1];
	m_jumping_towards = path[1];
	float x_dist = jump_loc.x - start.x;
	float y_dist = jump_loc.y - start.y;
	float desired_aim = atan2(y_dist, x_dist);
	m_aim_reason = JUMP;
	return desired_aim;
}

long FuzzyLogicAI::get_combo_id(const Player* player, const Weapon* weapon) {
	return (weapon->get_id() | (player->get_id() << 16));
}

void FuzzyLogicAI::populate_environment() {
	m_fuzzy_env.clear();
	
	const GameLogic* logic = get_logic();
	
	const Player* my_player = get_own_player();
	
	if (my_player == NULL) {
		return;
	}
	
	const Gate* enemy_gate = logic->get_map()->get_gate(get_other_team(my_player->get_team()));
	const Gate* allied_gate = logic->get_map()->get_gate(my_player->get_team());
	
	// Populate each category for each of the other players.
	ConstIterator<std::pair<uint32_t, Player*> > other_players = logic->list_players();
	
	while (other_players.has_more()) {
		std::pair<uint32_t, Player*> next_iter = other_players.next();
		
		Player* other_player = next_iter.second;
		if (other_player == my_player) {
			continue;
		}
		
		const Gate* other_enemy_gate = logic->get_map()->get_gate(get_other_team(other_player->get_team()));
		const Gate* other_allied_gate = logic->get_map()->get_gate(other_player->get_team());
		
		m_fuzzy_env.add_input(m_fuzzy->get_category_id("dist_to_other"), other_player, dist_between_players(my_player, other_player));
		m_fuzzy_env.add_input(m_fuzzy->get_category_id("dist_to_my_gate"), other_player, dist_to_own_gate(my_player));
		m_fuzzy_env.add_input(m_fuzzy->get_category_id("dist_to_enemy_gate"), other_player, dist_to_enemy_gate(my_player));
		m_fuzzy_env.add_input(m_fuzzy->get_category_id("other_dist_to_enemy_gate"), other_player, dist_to_enemy_gate(other_player));
		m_fuzzy_env.add_input(m_fuzzy->get_category_id("other_dist_to_own_gate"), other_player, dist_to_own_gate(other_player));
		m_fuzzy_env.add_input(m_fuzzy->get_category_id("holding_gate"), other_player, holding_gate(my_player));
		m_fuzzy_env.add_input(m_fuzzy->get_category_id("other_holding_gate"), other_player, holding_gate(other_player));
		m_fuzzy_env.add_input(m_fuzzy->get_category_id("my_energy_percent"), other_player, energy_percent(my_player));
		m_fuzzy_env.add_input(m_fuzzy->get_category_id("other_energy_percent"), other_player, energy_percent(other_player));
		m_fuzzy_env.add_input(m_fuzzy->get_category_id("gun_cooldown"), other_player, gun_cooldown(my_player));
		m_fuzzy_env.add_input(m_fuzzy->get_category_id("gun_angle_to_other"), other_player, gun_angle_to_player(my_player, other_player));
		m_fuzzy_env.add_input(m_fuzzy->get_category_id("time_to_impact"), other_player, time_to_impact(my_player));
		m_fuzzy_env.add_input(m_fuzzy->get_category_id("other_time_to_impact"), other_player, time_to_impact(other_player));
		m_fuzzy_env.add_input(m_fuzzy->get_category_id("can_see_player"), other_player, can_see_player(my_player, other_player));
		m_fuzzy_env.add_input(m_fuzzy->get_category_id("can_see_enemy_gate"), other_player, can_see_gate(my_player, enemy_gate));
		m_fuzzy_env.add_input(m_fuzzy->get_category_id("can_see_my_gate"), other_player, can_see_gate(my_player, allied_gate));
		m_fuzzy_env.add_input(m_fuzzy->get_category_id("other_can_see_enemy_gate"), other_player, can_see_gate(other_player, other_enemy_gate));
		m_fuzzy_env.add_input(m_fuzzy->get_category_id("other_can_see_own_gate"), other_player, can_see_gate(other_player, other_allied_gate));
		
		// Populate each weapon-choosing-related category for each of the other players.
		ConstIterator<Weapon*> weapons = logic->list_weapons();
	
		while (weapons.has_more()) {
			Weapon* weapon = weapons.next();
			
			m_fuzzy_env.add_input(m_fuzzy->get_category_id("weap_damage_at_player"), get_combo_id(other_player, weapon),  weapon->get_damage_at_point(my_player->get_x(), my_player->get_y(), other_player->get_x(), other_player->get_y()));
			m_fuzzy_env.add_input(m_fuzzy->get_category_id("weap_freeze_time"), get_combo_id(other_player, weapon),  weapon->get_freeze_time());
			m_fuzzy_env.add_input(m_fuzzy->get_category_id("weap_force"), get_combo_id(other_player, weapon),  weapon->get_force(my_player->get_x(), my_player->get_y(), other_player->get_x(), other_player->get_y()));
			m_fuzzy_env.add_input(m_fuzzy->get_category_id("gun_cooldown"), get_combo_id(other_player, weapon),  weapon->get_remaining_cooldown());
			m_fuzzy_env.add_input(m_fuzzy->get_category_id("other_holding_gate"), get_combo_id(other_player, weapon), holding_gate(my_player));
		}
	}	
}

float FuzzyLogicAI::find_desired_aim() {
	return m_fsm->find_desired_aim();
}

AI::AimReason FuzzyLogicAI::get_aim_reason() {
	return m_fsm->get_aim_reason();
}

int FuzzyLogicAI::get_curr_weapon() const {
	return m_curr_weapon;
}

const std::vector<SparseIntersectMap::Intersect>* FuzzyLogicAI::get_current_path() const {
	return &m_current_path;
}

float FuzzyLogicAI::get_last_aim() const {
	return m_last_aim;
}

float FuzzyLogicAI::get_max_aim_inaccuracy() const {
	return m_max_aim_inaccuracy;
}

float FuzzyLogicAI::get_curr_aim_inaccuracy() const {
	return m_aim_inaccuracy;
}

uint64_t FuzzyLogicAI::get_last_weapon_switch() const {
	return m_last_weapon_switch;
}

uint64_t FuzzyLogicAI::get_last_action() const {
	return m_last_action;
}

const SparseIntersectMap::Intersect& FuzzyLogicAI::get_jumping_towards() const {
	return m_jumping_towards;
}

bool FuzzyLogicAI::was_path_found() const {
	return m_found_path;
}

bool FuzzyLogicAI::was_grabbing() const {
	return m_was_grabbing;
}

bool FuzzyLogicAI::check_switch_weapons(const GameLogic& logic) {
	return false;
}

void FuzzyLogicAI::action_taken() {
	m_last_action = get_ticks();
}
