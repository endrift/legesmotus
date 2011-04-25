/*
 * ai/FuzzyLogicState.hpp
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

#ifndef LM_AI_FUZZYLOGICSTATE_HPP
#define LM_AI_FUZZYLOGICSTATE_HPP

#include "AI.hpp"

namespace LM {
	class FuzzyEnvironment;
	class FuzzyLogicAI;

	class FuzzyLogicState {
	private:

	public:
		virtual ~FuzzyLogicState() { }

		virtual const std::string& get_name() const = 0;
		virtual FuzzyLogicState* next_state() = 0;

		//virtual float get_max_aim_vel() const = 0;
		virtual float find_desired_aim() const = 0;
		virtual AI::AimReason get_aim_reason() const = 0;
		virtual int get_curr_weapon() const = 0;

		virtual void decide(FuzzyLogicAI* ai, FuzzyEnvironment* env, const GameLogic& logic) = 0;
		long get_combo_id(const Player* player, const Weapon* weapon);
	};
}

#endif
