/*
 * server/GameModeHelper.hpp
 *
 * This file is part of Leges Motus, a networked, 2D shooter set in zero gravity.
 * 
 * Copyright 2009 Andrew Ayer, Nathan Partlan, Jeffrey Pfau
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

#ifndef LM_SERVER_GAMEMODEHELPER_HPP
#define LM_SERVER_GAMEMODEHELPER_HPP

#include <stdint.h>
#include "common/GameParameters.hpp"

namespace LM {
	class Server;
	class ServerPlayer;

	class GameModeHelper {
	protected:
		Server&			m_server;

	public:
		virtual void		check_state() = 0;
		virtual void		gate_open(char team) = 0;
		virtual uint64_t	player_shot(ServerPlayer& shooter, ServerPlayer& shot_player) = 0;
		virtual void		game_timeout() = 0;
		virtual void		new_game() = 0;

		virtual bool		is_team_play() const = 0;
		virtual GameMode	get_type() const = 0;

		explicit GameModeHelper (Server& server) : m_server(server) { }
		virtual ~GameModeHelper () { }
	};
}

#endif
