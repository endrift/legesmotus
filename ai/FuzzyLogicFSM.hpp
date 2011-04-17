/*
 * ai/FuzzyLogicFSM.hpp
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

#ifndef LM_AI_FUZZYLOGICFSM_HPP
#define LM_AI_FUZZYLOGICFSM_HPP

#include "FuzzyLogicState.hpp"
#include "FuzzyEnvironment.hpp"
#include "common/FiniteStateMachine.hpp"

#include <map>

namespace LM {
	class Player;
	class FuzzyEnvironment;

	class FuzzyLogicFSM : public FiniteStateMachine<std::string, FuzzyLogicState> {
	private:
		const Player* m_curr_player;

	protected:
		virtual const std::string& get_transition();

	public:
		FuzzyLogicFSM(const std::string& start_state);

		void set_player(const Player* curr_player);
		const Player* get_player() const;

		float get_max_aim_vel() const;
		float find_desired_aim() const;
		AI::AimReason get_aim_reason() const;
		int get_curr_weapon() const;

		// TODO: figure out pathfinding location
		//Point desired_destination() const;

		void decide(FuzzyEnvironment* env, const GameLogic& logic);
	};
}

#endif
