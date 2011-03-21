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
	
	m_target = NULL;
	
	m_last_aim = 0.0f;
	
	// TODO: Load this from a config file?
	max_aim_inaccuracy = 0.2f;
	
	m_aim_reason = DO_NOTHING;
	
	m_found_path = false;
	m_current_path = NULL;
	
	initialize_logic();
}

FuzzyLogicAI::~FuzzyLogicAI() {
	delete m_fuzzy;
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
	
	// Load the rules.
	m_rule_dangerous = m_fuzzy->add_rule("dangerous", 
		new FuzzyLogic::Not(
			new FuzzyLogic::Or(
				new FuzzyLogic::And(
					m_fuzzy->make_terminal("can_see_player", "cant_see"), 
					m_fuzzy->make_terminal("other_holding_gate", "not_holding")
				),
				m_fuzzy->make_terminal("other_energy_percent","frozen")
			)
		)
	);
	
	m_rule_can_target = m_fuzzy->add_rule("can_target",
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
					new FuzzyLogic::Or(
						// Are they capturing our gate?
						new FuzzyLogic::Not(
							m_fuzzy->make_terminal("other_holding_gate", "not_holding")
						),
						// Is our gun turned far away?
						new FuzzyLogic::Not(
							m_fuzzy->make_terminal("gun_angle_to_other", "far_off")
						)
					)
				)
			),
			new FuzzyLogic::Or(
				m_fuzzy->make_terminal("gun_cooldown", "ready"),
				m_fuzzy->make_terminal("gun_cooldown", "almost_ready")
			)
		)
	);
	
	m_rule_firing_importance = m_fuzzy->add_rule("firing_importance",
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
							new FuzzyLogic::Or(
								// Are they capturing our gate?
								new FuzzyLogic::Not(
									m_fuzzy->make_terminal("other_holding_gate", "not_holding")
								),
								// Is our gun turned far away?
								new FuzzyLogic::Not(
									m_fuzzy->make_terminal("gun_angle_to_other", "far_off")
								)
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
		)
	);
	
	m_rule_run_away = m_fuzzy->add_rule("run_away",
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
		)
	);
	
	m_rule_jump_at_gate = m_fuzzy->add_rule("jump_at_gate",
		new FuzzyLogic::And(
			// Can we jump soon?
			new FuzzyLogic::Or(
				m_fuzzy->make_terminal("time_to_impact", "already_grabbing"),
				m_fuzzy->make_terminal("time_to_impact", "nearly_landed")
			),
			new FuzzyLogic::And(
				// Can we see the gate?
				new FuzzyLogic::Not(
					new FuzzyLogic::Or(
						m_fuzzy->make_terminal("can_see_enemy_gate", "far_away"),
						m_fuzzy->make_terminal("can_see_enemy_gate", "touching")
					)
				),
				new FuzzyLogic::And(
					// Are we already capturing their gate?
					new FuzzyLogic::Not(
						m_fuzzy->make_terminal("can_see_enemy_gate", "touching")
					),
					m_fuzzy->make_terminal("holding_gate", "not_holding")
				)
			)
		)
	);
	
	m_rule_dont_jump = m_fuzzy->add_rule("dont_jump",
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
			m_fuzzy->make_terminal("can_see_enemy_gate", "touching")
		)
	);
	
	//m_rule_touching_gate = m_fuzzy->add_rule("touching_gate", m_fuzzy->make_terminal("can_see_enemy_gate", "touching"));
}

void FuzzyLogicAI::randomize_aim_inaccuracy() {
	float uncertainty = max_aim_inaccuracy;
	m_aim_inaccuracy = (float)rand()/(float)RAND_MAX * uncertainty * 2 - uncertainty;
}

void FuzzyLogicAI::step(const GameLogic& logic, uint64_t diff) {
	populate_environment();
	
	const Player* my_player = get_own_player();
	
	if (my_player == NULL) {
		return;
	}
	
	m_fuzzy->apply(&m_fuzzy_env);
	
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
		
		float dangerous = m_fuzzy->decide(m_rule_dangerous, other_player, m_fuzzy_env);
		
		float can_target = m_fuzzy->decide(m_rule_can_target, other_player, m_fuzzy_env);

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
}

bool FuzzyLogicAI::set_path(b2Vec2 start, vector<SparseIntersectMap::Intersect>& path) {
	if (path.size() == 0) {
		m_current_path = NULL;
		return false;
	}
	m_current_path = &path;
	return true;
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
	}
}

float FuzzyLogicAI::find_desired_aim() {

	const GameLogic* logic = get_logic();
		
	const Player* my_player = get_own_player();

	const Gate* enemy_gate = logic->get_map()->get_gate(get_other_team(my_player->get_team()));

	int aim_at_target = 0;
	int aim_to_jump = 0;
	int aim_at_gate = 0;
	
	float desired_aim = 0.0f;
	
	if (m_target != NULL) {
		// Determine importance of aiming at target.
		float dangerous = m_fuzzy->decide(m_rule_dangerous, m_target, m_fuzzy_env);
		
		float can_target = m_fuzzy->decide(m_rule_can_target, m_target, m_fuzzy_env);
		
		float firing_importance = m_fuzzy->decide(m_rule_firing_importance, m_target, m_fuzzy_env);
		
		aim_at_target = (int)(dangerous * can_target * firing_importance * 100.0f);
		
		// Determine if we should jump at the gate.
		aim_at_gate = m_fuzzy->decide(m_rule_jump_at_gate, m_target, m_fuzzy_env) * 100;
	
		// Determine if we should run away.
		float run_away = m_fuzzy->decide(m_rule_run_away, m_target, m_fuzzy_env);
		float dont_jump = m_fuzzy->decide(m_rule_dont_jump, m_target, m_fuzzy_env);
		aim_to_jump = (run_away - dont_jump) * 100 - aim_at_target/2.0f;
		if (aim_to_jump < 0) {
			aim_to_jump = 0;
		}
		if (aim_to_jump < 0.1f && dont_jump <= 0.01f) {
			aim_to_jump = 30;
		}
		
		//DEBUG("Aim at target: " << aim_at_target);
	
	}
	
	// Re-find paths after we jump.
	if (!my_player->is_grabbing_obstacle()) {
		if (m_was_grabbing) {
			m_found_path = false;
		}
	}
	m_was_grabbing = my_player->is_grabbing_obstacle();
	
	int total = aim_at_target + aim_to_jump + aim_at_gate;
	if (total < 20) {
		// Do nothing.
		m_aim_reason = DO_NOTHING;
		return m_last_aim;
	} else {
		int result = rand() % total;
		if (result < aim_at_target) {
			// Aim at the target.
			// Find the angle to turn towards the enemy.
			float x_dist = m_target->get_x() - my_player->get_x();
			float y_dist = m_target->get_y() - my_player->get_y();
			desired_aim = atan2(y_dist, x_dist) + m_aim_inaccuracy;
			m_aim_reason = FIRE;
		} else if (result < aim_at_target + aim_to_jump) {
			// Aim to jump.
			// 1/5th chance to jump towards the gate, even if we can't see it.
			if (m_aim_reason != JUMP) {
				if (rand() % 5 == 1) {
					Point gate_pos = enemy_gate->get_position();
					float x_dist = gate_pos.x - my_player->get_x();
					float y_dist = gate_pos.y - my_player->get_y();
					desired_aim = atan2(y_dist, x_dist);
					m_aim_reason = JUMP;
				} else {
					// For now, just randomly aim somewhere.
					desired_aim = to_radians(rand()%360 - 180);
					m_aim_reason = JUMP;
				}
				if (my_player->is_grabbing_obstacle()) {
					std::vector<SparseIntersectMap::Intersect> path;
					b2Vec2 start(0,0);
					m_found_path = find_path(my_player, enemy_gate->get_position().x, enemy_gate->get_position().y, 50, path, &start);
					if (set_path(start, path)) {
						desired_aim = get_next_aim(start, path);
					}
				}
			} else {
				m_aim_reason = JUMP;
				
				if (my_player->is_grabbing_obstacle() && !m_found_path) {
					std::vector<SparseIntersectMap::Intersect> path;
					b2Vec2 start(0,0);
					m_found_path = find_path(my_player, enemy_gate->get_position().x, enemy_gate->get_position().y, 50, path, &start);
					if (set_path(start, path)) {
						desired_aim = get_next_aim(start, path);
					} else {
						return m_last_aim;
					}
				} else {
					return m_last_aim;
				}
			}
		} else {
			Point gate_pos = enemy_gate->get_position();
			float x_dist = gate_pos.x - my_player->get_x();
			float y_dist = gate_pos.y - my_player->get_y();
			desired_aim = atan2(y_dist, x_dist);
			
			// Aim to jump at the gate.
			if (!m_found_path) {
				std::vector<SparseIntersectMap::Intersect> path;
				b2Vec2 start(0,0);
				m_found_path = find_path(my_player, enemy_gate->get_position().x, enemy_gate->get_position().y, 50, path, &start);
				if (set_path(start, path)) {
					desired_aim = get_next_aim(start, path);
				}
			}
		}
	}
	
	m_last_aim = desired_aim;
	return desired_aim;
}

AI::AimReason FuzzyLogicAI::get_aim_reason() {
	return m_aim_reason;
}

const std::vector<SparseIntersectMap::Intersect>* FuzzyLogicAI::get_current_path() const {
	return m_current_path;
}
