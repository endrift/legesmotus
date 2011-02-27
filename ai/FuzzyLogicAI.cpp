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

using namespace LM;
using namespace std;

FuzzyLogicAI::FuzzyLogicAI(const Configuration* config, const GameLogic* logic) : AI(logic) {
	m_fuzzy = new FuzzyLogic("default");
	m_config = config;

	m_fuzzy->load_category(config, "can_see_player");
}

FuzzyLogicAI::~FuzzyLogicAI() {
	delete m_fuzzy;
}

void FuzzyLogicAI::update(const GameLogic& logic, uint64_t diff) {
	set_logic(&logic);
	populate_environment();
}

void FuzzyLogicAI::populate_environment() {
	m_fuzzy_env.clear();
	
	const Player* my_player = get_own_player();
	
	if (my_player == NULL) {
		return;
	}
	
	const Player* other_player = get_other_player();
	
	if (other_player == NULL) {
		return;
	}
	
	const Gate* enemy_gate = get_logic()->get_map()->get_gate(get_other_team(my_player->get_team()));
	const Gate* allied_gate = get_logic()->get_map()->get_gate(my_player->get_team());
	const Gate* other_enemy_gate = get_logic()->get_map()->get_gate(get_other_team(other_player->get_team()));
	const Gate* other_allied_gate = get_logic()->get_map()->get_gate(other_player->get_team());
	
	/*m_fuzzy_env.set_input(m_fuzzy->get_category_id("dist_to_other"), dist_between_players(my_player, other_player));
	m_fuzzy_env.set_input(m_fuzzy->get_category_id("dist_to_my_gate"), dist_to_own_gate(my_player));
	m_fuzzy_env.set_input(m_fuzzy->get_category_id("dist_to_enemy_gate"), dist_to_enemy_gate(my_player));
	m_fuzzy_env.set_input(m_fuzzy->get_category_id("other_dist_to_enemy_gate"), dist_to_enemy_gate(other_player));
	m_fuzzy_env.set_input(m_fuzzy->get_category_id("other_dist_to_own_gate"), dist_to_own_gate(other_player));
	m_fuzzy_env.set_input(m_fuzzy->get_category_id("holding_gate"), holding_gate(my_player));
	m_fuzzy_env.set_input(m_fuzzy->get_category_id("other_holding_gate"), holding_gate(other_player));
	m_fuzzy_env.set_input(m_fuzzy->get_category_id("my_energy_percent"), energy_percent(my_player));
	m_fuzzy_env.set_input(m_fuzzy->get_category_id("other_energy_percent"), energy_percent(other_player));
	m_fuzzy_env.set_input(m_fuzzy->get_category_id("gun_cooldown"), gun_cooldown(my_player));
	m_fuzzy_env.set_input(m_fuzzy->get_category_id("gun_angle_to_other"), gun_angle_to_player(my_player, other_player));
	m_fuzzy_env.set_input(m_fuzzy->get_category_id("time_to_impact"), time_to_impact(my_player));
	m_fuzzy_env.set_input(m_fuzzy->get_category_id("other_time_to_impact"), time_to_impact(other_player));*/
	m_fuzzy_env.set_input(m_fuzzy->get_category_id("can_see_player"), can_see_player(my_player, other_player));
	/*m_fuzzy_env.set_input(m_fuzzy->get_category_id("can_see_enemy_gate"), can_see_gate(my_player, enemy_gate));
	m_fuzzy_env.set_input(m_fuzzy->get_category_id("can_see_my_gate"), can_see_gate(my_player, allied_gate));
	m_fuzzy_env.set_input(m_fuzzy->get_category_id("other_can_see_enemy_gate"), can_see_gate(my_player, other_enemy_gate));
	m_fuzzy_env.set_input(m_fuzzy->get_category_id("other_can_see_own_gate"), can_see_gate(my_player, other_allied_gate));*/
}

float FuzzyLogicAI::find_desired_aim() {
	return 0;
}

AI::AimReason FuzzyLogicAI::get_aim_reason() {
	return DO_NOTHING;
}
