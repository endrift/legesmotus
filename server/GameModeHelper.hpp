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
		// Called once every time through the server loop.
		// The game mode should use it to determine whether the game is still in play.
		virtual void		check_state() = 0;

		// Called when the given team's gate has been fully opened.
		virtual void		gate_open(char team) = 0;

		// Called when one player is shot by another player.
		// Returns true if the shot resulted in damage, false otherwise.
		virtual bool		player_shot(ServerPlayer& shooter, ServerPlayer& shot_player) = 0;

		// Called when a player is killed (i.e. energy reaches 0).
		// killer is the player who dealt the last blow (or NULL if no player did it, e.g. a hazardous map object)
		// The return value is the amount of time the player should be frozen (may be 0).
		virtual uint64_t	player_died(ServerPlayer* killer, ServerPlayer& killed) = 0;

		// Called when the game times out.
		virtual void		game_timeout() = 0;

		// Called when a new game starts (specifically, from Server::new_game).
		virtual void		new_game() = 0;

		// Is this a team-based game?
		//  If true, players are distributed between blue and red, and allowed to switch.
		//  If false, all players are assigned to blue (A).
		virtual bool		is_team_play() const = 0;

		// Return the game mode type, as defined in common/GameParameters.hpp
		virtual GameMode	get_type() const = 0;

		explicit GameModeHelper (Server& server) : m_server(server) { }
		virtual ~GameModeHelper () { }
	};
}

#endif
