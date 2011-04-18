/*
 * ai/FuzzyLogicFSM.cpp
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

#include "FuzzyLogicFSM.hpp"

using namespace LM;
using namespace std;

FuzzyLogicFSM::FuzzyLogicFSM(const string& start_state) : FiniteStateMachine<string, FuzzyLogicState>(start_state) {
	// Nothing to do
}

const string& FuzzyLogicFSM::get_transition() {
	return get_current_state_data()->next_state()->get_name();
}

float FuzzyLogicFSM::get_max_aim_vel() const {
	return get_current_state_data()->get_max_aim_vel();
}

float FuzzyLogicFSM::find_desired_aim() const {
	return get_current_state_data()->find_desired_aim();
}

AI::AimReason FuzzyLogicFSM::get_aim_reason() const {
	return get_current_state_data()->get_aim_reason();
}

int FuzzyLogicFSM::get_curr_weapon() const {
	return get_current_state_data()->get_curr_weapon();
}

void FuzzyLogicFSM::decide(FuzzyLogicAI* ai, FuzzyEnvironment* env, const GameLogic& logic) {
	return get_current_state_data()->decide(ai, env, logic);
}
