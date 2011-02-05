/*
 * ai/ReactiveAIController.hpp
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

#ifndef LM_AI_REACTIVEAICONTROLLER_HPP
#define LM_AI_REACTIVEAICONTROLLER_HPP

#include "client/Controller.hpp"
#include <string>

namespace LM {
	class ReactiveAIController : public Controller {
	
	private:
		const static float MAX_AIM_VEL;
		const static unsigned int AIM_TOLERANCE;
	
		int m_changes[2];
		int m_changeset;
		
		float m_wanted_aim; // Radians
		float m_curr_aim; // Radians
		
		void find_desired_aim(const GameLogic& state);
		
		void update_gun();

	public:
		ReactiveAIController();
		
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
