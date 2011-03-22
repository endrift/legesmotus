/*
 * ai/AIController.cpp
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

#include "AIController.hpp"

using namespace LM;
using namespace std;

const float AIController::MAX_AIM_VEL = .03f;
const unsigned int AIController::AIM_TOLERANCE = .01f;

AIController::AIController(AI* ai) {
	m_aim_reason = AI::DO_NOTHING;
	
	m_ai = ai;
	
	m_changeset = 0;
	
	m_wanted_aim = 0;
	m_curr_aim = 0;
	m_aim_reason = AI::DO_NOTHING;
}

float AIController::update_gun() {
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
	
	return fabs(m_curr_aim - m_wanted_aim);
}

void AIController::update(uint64_t diff, const GameLogic& state, int player_id) {
	m_changes[m_changeset] = NO_CHANGE;
	m_changeset ^= 1;
	
	const Player* my_player = state.get_player(player_id);
	if (my_player == NULL) {
		return;
	}
	
	if (my_player->is_frozen()) {
		return;
	}
	
	if (m_ai != NULL) {
		m_ai->set_logic(&state);
		m_ai->set_own_player(my_player);
	
		// Update the AI state:
		m_ai->update(state, diff);
		
		// Determine new desired aim.
		m_wanted_aim = m_ai->find_desired_aim();
		m_aim_reason = m_ai->get_aim_reason();
	}
	
	// Turn gun towards wanted aim.
	float aimdiff = update_gun();
	
	if (aimdiff <= AIM_TOLERANCE) {
		if (m_aim_reason == AI::FIRE) {
			m_changes[m_changeset ^ 1] |= FIRE_WEAPON;
			m_changes[m_changeset ^ 1] |= STOP_JUMPING;
			m_ai->randomize_aim_inaccuracy();
		} else if (m_aim_reason == AI::JUMP && my_player->is_grabbing_obstacle()) {
			m_changes[m_changeset ^ 1] |= JUMPING;
		} else {
			m_changes[m_changeset ^ 1] |= STOP_JUMPING;
		}
	} else {
		m_changes[m_changeset ^ 1] |= STOP_JUMPING;
	}
}

int AIController::get_changes() const {
	return m_changes[m_changeset];
}

float AIController::get_aim() const {
	return m_curr_aim;
}

float AIController::get_distance() const {
	return 0;
}

int AIController::get_weapon() const {
	return 0;
}

wstring AIController::get_message() const {
	return NULL;
}

bool AIController::message_is_team_only() const {
	return false;
}

void AIController::received_message(const Player* p, const wstring& message) {
}
