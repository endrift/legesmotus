/*
 * ai/DefensiveState.cpp
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

#include "DefensiveState.hpp"
#include "common/Weapon.hpp"
#include "FuzzyLogicAI.hpp"
#include "common/team.hpp"

using namespace LM;
using namespace std;

DefensiveState::DefensiveState(const FuzzyLogic* fuzzy_logic) {
	m_name = "";
	m_next_state = this;
	
	m_fuzzy = fuzzy_logic;
	
	m_target = NULL;
	m_wanted_weapon = 0;
	m_desired_aim = 0;
	m_aim_reason = AI::DO_NOTHING;
	
	load_rules();
}

DefensiveState::~DefensiveState() {

}

void DefensiveState::load_rules() {
	// Load the rules.
	m_rule_dangerous =
		new FuzzyLogic::Not(
			new FuzzyLogic::Or(
				new FuzzyLogic::And(
					new FuzzyLogic::And(
						m_fuzzy->make_terminal("can_see_player", "cant_see"), 
						m_fuzzy->make_terminal("other_holding_gate", "not_holding")
					),
					m_fuzzy->make_terminal("other_can_see_enemy_gate", "far_away")
				),
				m_fuzzy->make_terminal("other_energy_percent","frozen")
			)
		);
	
	m_rule_can_target =
		new FuzzyLogic::And(
			new FuzzyLogic::And(
				new FuzzyLogic::Not(
					m_fuzzy->make_terminal("can_see_player", "cant_see")
				),
				new FuzzyLogic::Or(
					// Are we close enough to hit them accurately?
					new FuzzyLogic::Not(
						m_fuzzy->make_terminal("dist_to_other", "far_away")
					),
					// Are they capturing our gate?
					new FuzzyLogic::Not(
						m_fuzzy->make_terminal("other_holding_gate", "not_holding")
					)
				)
			),
			new FuzzyLogic::Or(
				m_fuzzy->make_terminal("gun_cooldown", "ready"),
				m_fuzzy->make_terminal("gun_cooldown", "almost_ready")
			)
		);
	
	m_rule_firing_importance =
		new FuzzyLogic::And(
			new FuzzyLogic::And(
				// Are we in danger of dying soon?
				new FuzzyLogic::Not(
					new FuzzyLogic::Or(
						m_fuzzy->make_terminal("my_energy_percent", "frozen"),
						m_fuzzy->make_terminal("my_energy_percent", "almost_frozen")
					)
				),
				// Are we ready to fire at them?
				new FuzzyLogic::And(
					new FuzzyLogic::Or(
						m_fuzzy->make_terminal("gun_cooldown", "ready"),
						m_fuzzy->make_terminal("gun_cooldown", "almost_ready")
					),
					new FuzzyLogic::Or(
						new FuzzyLogic::Or(
							// Are we capturing their gate?
							m_fuzzy->make_terminal("can_see_enemy_gate", "touching"),
							new FuzzyLogic::Not(
								m_fuzzy->make_terminal("holding_gate", "not_holding")
							)
						),
						new FuzzyLogic::Or(
							// Are we close enough to hit them accurately?
							new FuzzyLogic::Not(
								m_fuzzy->make_terminal("dist_to_other", "far_away")
							),
							// Are they capturing our gate?
							new FuzzyLogic::Not(
								m_fuzzy->make_terminal("other_holding_gate", "not_holding")
							)
						)
					)
				)
			),
			new FuzzyLogic::Or(
				new FuzzyLogic::Or(
					// Are we capturing their gate?
					m_fuzzy->make_terminal("can_see_enemy_gate", "touching"),
					new FuzzyLogic::Not(
						m_fuzzy->make_terminal("holding_gate", "not_holding")
					)
				),
				new FuzzyLogic::Or(
					// Are they capturing our gate?
					new FuzzyLogic::Not(
						m_fuzzy->make_terminal("other_holding_gate", "not_holding")
					),
					// Are we not needing to jump?
					new FuzzyLogic::Not(
						new FuzzyLogic::Or(
							m_fuzzy->make_terminal("time_to_impact", "already_grabbing"),
							m_fuzzy->make_terminal("time_to_impact", "nearly_landed")
						)
					)
				)
			)
		);
	
	m_rule_run_away = 
		new FuzzyLogic::And(
			// Can we jump soon?
			new FuzzyLogic::Or(
				m_fuzzy->make_terminal("time_to_impact", "already_grabbing"),
				m_fuzzy->make_terminal("time_to_impact", "nearly_landed")
			),
			new FuzzyLogic::And(
				// Are we in danger?
				new FuzzyLogic::Or(
					new FuzzyLogic::Or(
						m_fuzzy->make_terminal("my_energy_percent", "frozen"),
						m_fuzzy->make_terminal("my_energy_percent", "almost_frozen")
					),
					// Are they going to kill us soon?
					new FuzzyLogic::Or(
						new FuzzyLogic::Not(
							new FuzzyLogic::Or(
								m_fuzzy->make_terminal("gun_cooldown", "ready"),
								m_fuzzy->make_terminal("gun_cooldown", "almost_ready")
							)
						),
						m_fuzzy->make_terminal("gun_angle_to_other", "far_off")
					)
				),
				// Are they capturing our gate?
				m_fuzzy->make_terminal("other_holding_gate", "not_holding")
			)
		);
	
	m_rule_dont_jump = 
		new FuzzyLogic::Or(
			new FuzzyLogic::And(
				// Is our cooldown nearly ready for firing?
				new FuzzyLogic::Or(
					m_fuzzy->make_terminal("gun_cooldown", "ready"),
					m_fuzzy->make_terminal("gun_cooldown", "almost_ready")
				),
				// Can we jump soon?
				new FuzzyLogic::Not(
					new FuzzyLogic::Or(
						m_fuzzy->make_terminal("time_to_impact", "already_grabbing"),
						m_fuzzy->make_terminal("time_to_impact", "nearly_landed")
					)
				)
			),
			// Are we already grabbing the gate?
			new FuzzyLogic::Or(
				m_fuzzy->make_terminal("can_see_enemy_gate", "touching"),
				new FuzzyLogic::Not(
					m_fuzzy->make_terminal("holding_gate", "not_holding")
				)
			)
		);
	
	m_rule_jump_own_gate =
		new FuzzyLogic::And(
			new FuzzyLogic::And(
				// Can we see our enemy?
				m_fuzzy->make_terminal("can_see_player", "cant_see"),
				// Are we ready to jump?
				new FuzzyLogic::Or(
					m_fuzzy->make_terminal("time_to_impact", "already_grabbing"),
					m_fuzzy->make_terminal("time_to_impact", "nearly_landed")
				)
			),
			// Can we see our own gate?
			m_fuzzy->make_terminal("can_see_my_gate", "far_away")
		);
			
	m_rule_weapon_fitness = 
		new FuzzyLogic::Or(
			// Will it freeze them?
			new FuzzyLogic::And(
				m_fuzzy->make_terminal("weap_damage_at_player", "freeze"),
				new FuzzyLogic::Or(
					m_fuzzy->make_terminal("gun_cooldown", "ready"),
					m_fuzzy->make_terminal("gun_cooldown", "almost_ready")
				)
			),
			// Will it briefly freeze them at least, while they're holding our gate?
			new FuzzyLogic::And(
				new FuzzyLogic::Not(
					m_fuzzy->make_terminal("other_holding_gate", "not_holding")
				),
				new FuzzyLogic::Not(
					m_fuzzy->make_terminal("weap_freeze_time", "none")
				)
			)
		);
}

const string& DefensiveState::get_name() const {
	return m_name;
}

FuzzyLogicState* DefensiveState::next_state() {
	return m_next_state;
}

float DefensiveState::find_desired_aim() const {
	return m_desired_aim;
}

AI::AimReason DefensiveState::get_aim_reason() const {
	return m_aim_reason;
}

int DefensiveState::get_curr_weapon() const {
	return m_wanted_weapon;
}

void DefensiveState::decide(FuzzyLogicAI* ai, FuzzyEnvironment* env, const GameLogic& logic) {
	const Player* my_player = ai->get_own_player();

	ConstIterator<std::pair<uint32_t, Player*> > other_players = logic.list_players();
	Player* best_target = NULL;
	float best_target_val = 0.0f;
	// Determine danger for each enemy player.
	while (other_players.has_more()) {
		std::pair<uint32_t, Player*> next_iter = other_players.next();
		
		Player* other_player = next_iter.second;
		if (other_player == my_player) {
			continue;
		}
		
		// Ignore my own team.
		if (other_player->get_team() == my_player->get_team()) {
			continue;
		}
		
		float dangerous = m_rule_dangerous->apply(*env, (long)other_player);
		
		float can_target = m_rule_can_target->apply(*env, (long)other_player);
		
		float target_val = dangerous * can_target;
		
		if (target_val > best_target_val || best_target == NULL) {
			best_target = other_player;
			best_target_val = target_val;
		} else if (target_val == best_target_val) {
			// If equally dangerous, choose randomly between them for now.
			if (rand()%2 == 0) {
				best_target = other_player;
				best_target_val = target_val;
			}
		}
	}
	
	m_target = best_target;
	
	// Check if we should switch weapons.
	check_switch_weapons(ai, logic, env);

	// Update our wanted aim.
	update_wanted_aim(ai, logic, env);
}

bool DefensiveState::check_switch_weapons(FuzzyLogicAI* ai, const GameLogic& logic, FuzzyEnvironment* env) {
	if (ai->get_last_weapon_switch() < get_ticks() - FuzzyLogicAI::MAX_WEAPON_SWITCH_FREQ && m_target != NULL) {
		// Determine whether we should switch weapons.
		int total_weapon_val = 0;
		
		int weapon_vals[logic.num_weapons()];
		
		int best_weapon = ai->get_curr_weapon();

		
		ConstIterator<Weapon*> weapons = logic.list_weapons();

		while (weapons.has_more()) {
			Weapon* weapon = weapons.next();
		
			float weapon_fitness = m_rule_weapon_fitness->apply(*env, get_combo_id(m_target, weapon));
		
			// Favor the current weapon.
			if (weapon->get_id() == ai->get_curr_weapon() && weapon_fitness >= 0.95f) {
				weapon_fitness *= 50.0f;
			}
			
			total_weapon_val += weapon_fitness * 100.0f;
			weapon_vals[weapon->get_id()] = weapon_fitness * 100.0f;
		}
		
		if (total_weapon_val != 0) {
			int result = rand() % total_weapon_val;
			int i = 0;
			//DEBUG("Total: " << total_weapon_val << " Result: " << result);
			while(result > 0 && i < logic.num_weapons()) {
				result -= weapon_vals[i];
				if (result <= 0) {
					best_weapon = i;
				}
				i++;
			}
		}
	
		m_wanted_weapon = best_weapon;
	}
	
	return ai->get_curr_weapon() != m_wanted_weapon;
}

void DefensiveState::update_wanted_aim(FuzzyLogicAI* ai, const GameLogic& logic, FuzzyEnvironment* env) {
		
	const Player* my_player = ai->get_own_player();

	const Gate* my_gate = logic.get_map()->get_gate(my_player->get_team());

	int aim_at_target = 0;
	int aim_to_jump = 0;
	int aim_at_gate = 0;
	
	float desired_aim = 0.0f;
	
	if (m_target != NULL) {
		// Determine importance of aiming at target.
		float dangerous = m_rule_dangerous->apply(*env, (long)m_target);
		
		float can_target = m_rule_can_target->apply(*env, (long)m_target);
		
		float firing_importance = m_rule_firing_importance->apply(*env, (long)m_target);
		
		aim_at_target = (int)(dangerous * can_target * firing_importance * 100.0f);
		
		// Determine if we should jump at the gate.
		aim_at_gate = m_rule_jump_own_gate->apply(*env, (long)m_target) * 100;
	
		// Determine if we should run away.
		float run_away = m_rule_run_away->apply(*env, (long)m_target);
		float dont_jump = m_rule_dont_jump->apply(*env, (long)m_target);
		aim_to_jump = (run_away - dont_jump) * 100 - aim_at_target/2.0f;
		if (aim_to_jump < 0) {
			aim_to_jump = 0;
		}
		if (aim_to_jump < 0.1f && dont_jump <= 0.01f) {
			aim_to_jump = 30;
		}
	}
	
	// Re-find paths after we jump.
	if (!my_player->is_grabbing_obstacle()) {
		if (ai->was_grabbing()) {
			ai->set_found_path(false);
		}
	}
	
	Pathfinder* pathfinder = ai->get_pathfinder();
	
	const std::vector<SparseIntersectMap::Intersect>* current_path = ai->get_current_path();
	
	if (current_path->size() > 0) {
		Pathfinder::AvoidArea avoidlast;
		avoidlast.x = (*current_path)[0].x;
		avoidlast.y = (*current_path)[0].y;
		avoidlast.area = FuzzyLogicAI::AREA_AVOID_SIZE;
		avoidlast.weight = FuzzyLogicAI::AREA_AVOID_WEIGHT;
		pathfinder->add_avoid_area(&avoidlast);
	}
	
	int total = aim_at_target + aim_to_jump + aim_at_gate;
	if (total == 0) {
		total = 1;
	}
	int result = rand() % total;
	
	bool take_action = true;
	
	if (total < 20) {
		if (ai->get_last_action() < get_ticks() - FuzzyLogicAI::ALLOWED_IDLE_TIME) {
			// We've idled too long. Aim to jump.
			result = aim_at_target + 1;
		} else {
			// Do nothing.
			m_aim_reason = AI::DO_NOTHING;
			desired_aim = ai->get_last_aim();
			take_action = false;
		}
	}
	
	if (take_action) {
		ai->action_taken();
		
		if (result < aim_at_target) {
			// Aim at the target.
			// Find the angle to turn towards the enemy.
			float x_dist = m_target->get_x() - my_player->get_x();
			float y_dist = m_target->get_y() - my_player->get_y();
			desired_aim = atan2(y_dist, x_dist) + ai->get_curr_aim_inaccuracy();
			m_aim_reason = AI::FIRE;
		} else if (result < aim_at_target + aim_to_jump) {
			// Aim to jump.
			if (m_aim_reason != AI::JUMP) {
				// For now, just randomly aim somewhere.
				desired_aim = to_radians(rand()%360 - 180);
				m_aim_reason = AI::JUMP;
			} else {
				m_aim_reason = AI::JUMP;
				
				desired_aim = ai->get_last_aim();
			}
		} else {
			Point gate_pos = my_gate->get_position();
			float x_dist = gate_pos.x - my_player->get_x();
			float y_dist = gate_pos.y - my_player->get_y();
			desired_aim = atan2(y_dist, x_dist);
			
			// Aim to jump at the gate.
			if (my_player->is_grabbing_obstacle()) {
				std::vector<SparseIntersectMap::Intersect> path;
				b2Vec2 start(0,0);
				ai->find_path(my_player, my_gate->get_position().x, my_gate->get_position().y, 50, path, &start);
				if (ai->set_path(start, path)) {
					desired_aim = ai->get_next_aim(start, path);
				}
			}
		}
	}
	
	pathfinder->clear_avoid_areas();
	
	m_desired_aim = desired_aim;
}
