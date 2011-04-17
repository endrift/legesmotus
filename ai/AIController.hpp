/*
 * ai/AIController.hpp
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

#ifndef LM_AI_AICONTROLLER_HPP
#define LM_AI_AICONTROLLER_HPP

#include "client/Controller.hpp"
#include "AI.hpp"

namespace LM {
	class AIController : public Controller {
	private:
		float m_max_aim_vel;
		const static unsigned int AIM_TOLERANCE;
	
		AI* m_ai;
	
		int m_changes[2];
		int m_changeset;
		
		int m_current_weapon;
		
		float m_wanted_aim; // Radians
		float m_curr_aim; // Radians
		AI::AimReason m_aim_reason; // Why are we aiming towards the desired location?

		float update_gun(); // Returns the absolute value of the difference between desired and actual angle.

	public:
		AIController(AI* ai);
	
		virtual void update(uint64_t diff, const GameLogic& state, int player_id);

		virtual int get_changes() const;
		virtual float get_aim() const;
		virtual float get_distance() const;
		virtual int get_weapon() const;

		virtual std::wstring get_message() const;
		virtual bool message_is_team_only() const;
		virtual void received_message(const Player* p, const std::wstring& message);
	};
}

#endif
