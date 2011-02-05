/*
 * newclient/Controller.hpp
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

#ifndef LM_NEWCLIENT_CONTROLLER_HPP
#define LM_NEWCLIENT_CONTROLLER_HPP

#include <string>
#include <stdint.h>

#include "common/GameLogic.hpp"

namespace LM {
	class Player;
	class Map;

	class Controller {
	public:
		enum {
			NO_CHANGE        = 0x00,
			JUMPING          = 0x01,
			STOP_JUMPING     = 0x02,
			CHANGE_AIM       = 0x04,
			CHANGE_WEAPON    = 0x08,
			FIRE_WEAPON      = 0x10,
			STOP_FIRE_WEAPON = 0x20,
			SEND_MESSAGE     = 0x40
		};

		virtual ~Controller() {}

		virtual void update(uint64_t diff, const GameLogic& state, int player_id) = 0;

		virtual int get_changes() const = 0;
		virtual float get_aim() const = 0;
		virtual float get_distance() const = 0;
		virtual int get_weapon() const = 0;

		virtual std::wstring get_message() const = 0;
		virtual bool message_is_team_only() const = 0;
		virtual void received_message(const Player* p, const std::wstring& message) = 0;
	};
}

#endif
