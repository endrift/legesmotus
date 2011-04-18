/*
 * ai/FuzzyLogicAI.hpp
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

#ifndef LM_AI_FUZZYLOGICAI_HPP
#define LM_AI_FUZZYLOGICAI_HPP

#include "AI.hpp"
#include "FuzzyLogic.hpp"
#include "common/Configuration.hpp"
#include "common/GameLogic.hpp"
#include <vector>

namespace LM {
	class Pathfinder;

	class FuzzyLogicAI : public AI {
	private:
		const static float AREA_AVOID_WEIGHT;
		const static float AREA_AVOID_SIZE;
		const static uint64_t ALLOWED_IDLE_TIME;
		const static uint64_t MAX_WEAPON_SWITCH_FREQ;
	
		FuzzyLogic* m_fuzzy;
		FuzzyEnvironment m_fuzzy_env;
		const Configuration* m_config;
		Player* m_target;
		float m_last_aim;
		float m_max_aim_inaccuracy;
		float m_aim_inaccuracy;
		uint64_t m_last_weapon_switch;
		uint64_t m_last_action;
		
		SparseIntersectMap::Intersect m_jumping_towards;
		std::vector<SparseIntersectMap::Intersect> m_current_path;
		bool m_found_path;
		bool m_was_grabbing;

		// Cached state
		float m_max_aim_vel;
		float m_desired_aim;
		AI::AimReason m_aim_reason;
		int m_curr_weapon;
		
		// Fuzzy Logic Rules
		int m_rule_dangerous;
		int m_rule_can_target;
		int m_rule_firing_importance;
		int m_rule_run_away;
		int m_rule_jump_at_gate;
		int m_rule_dont_jump;
		int m_rule_weapon_fitness;

		void initialize_logic();
		void populate_environment();

		virtual void step(const GameLogic& logic, uint64_t diff);
		
		virtual bool set_path(b2Vec2 start, std::vector<SparseIntersectMap::Intersect>& path);
		virtual float get_next_aim(b2Vec2 start, std::vector<SparseIntersectMap::Intersect>& path);
		virtual bool check_switch_weapons(const GameLogic& logic);

		// For putting values in the FuzzyLogic that have both player and weapon ID dependencies:
		long get_combo_id(const Player* player, const Weapon* weapon);
	public:
		FuzzyLogicAI(const Configuration* config, const GameLogic* logic = NULL);
		virtual ~FuzzyLogicAI();
		
		virtual const std::vector<SparseIntersectMap::Intersect>* get_current_path() const;

		// TODO: make these const
		virtual void randomize_aim_inaccuracy();
		virtual float find_desired_aim();
		virtual AimReason get_aim_reason();
		
		virtual int get_curr_weapon() const;

		float get_last_aim() const;
		float get_max_aim_inaccuracy() const;
		uint64_t get_last_weapon_switch() const;
		uint64_t get_last_action() const;
		const SparseIntersectMap::Intersect& get_jumping_towards() const;
		bool was_path_found() const;
		bool was_grabbing() const;
	};
}

#endif
