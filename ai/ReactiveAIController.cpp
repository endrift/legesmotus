/*
 * ai/ReactiveAIController.cpp
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

#include "ReactiveAIController.hpp"
#include <cstdlib>

using namespace LM;
using namespace std;

const float ReactiveAIController::MAX_AIM_VEL = .05f;
const unsigned int ReactiveAIController::AIM_TOLERANCE = .01f;

ReactiveAIController::ReactiveAIController() {
	srand(time(NULL));

	m_changeset = 0;
	
	m_wanted_aim = 0;
	m_curr_aim = 0;
}

void ReactiveAIController::find_desired_aim(const GameLogic& state) {
	// For now, just randomly aim somewhere.
	// XXX: Can we lock this to framerate better?
	if (rand()%60 == 1) {
		m_wanted_aim = to_radians(rand()%360 - 180);
	}
}

void ReactiveAIController::update_gun() {
	float aimdiff = fabs(m_curr_aim - m_wanted_aim);
	
	if (aimdiff > AIM_TOLERANCE) {
		m_changes[m_changeset ^ 1] |= CHANGE_AIM;
		int dir = m_curr_aim > m_wanted_aim ? -1 : 1;
		
		// Avoid going the long way around.
		if (aimdiff > M_PI) {
			m_curr_aim += 2 * M_PI * dir;
			dir *= -1;
		}
		
		m_curr_aim += dir * min(MAX_AIM_VEL, dir * (m_wanted_aim - m_curr_aim));
	}
}

void ReactiveAIController::update(uint64_t diff, const GameLogic& state, int player_id) {
	m_changes[m_changeset] = NO_CHANGE;
	m_changeset ^= 1;
	
	// Determine desired aim.
	find_desired_aim(state);
	
	// Turn gun towards wanted aim.
	update_gun();
	
	// Determine whether to jump.
	// XXX: Testing code: For now, just jump like crazy!
	m_changes[m_changeset ^ 1] |= JUMPING;
	// XXX: End testing code.
}

int ReactiveAIController::get_changes() const {
	return m_changes[m_changeset];
}

float ReactiveAIController::get_aim() const {
	return m_curr_aim;
}

float ReactiveAIController::get_distance() const {
	return 0;
}

int ReactiveAIController::get_weapon() const {
	return 0;
}

wstring ReactiveAIController::get_message() const {
	return NULL;
}

bool ReactiveAIController::message_is_team_only() const {
	return false;
}

void ReactiveAIController::received_message(const Player* p, const wstring& message) {
}
