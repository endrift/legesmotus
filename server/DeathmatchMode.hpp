/*
 * server/DeathmatchMode.hpp
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

#ifndef LM_SERVER_DEATHMATCHMODE_HPP
#define LM_SERVER_DEATHMATCHMODE_HPP

#include "GameModeHelper.hpp"

namespace LM {
	class DeathmatchMode : public GameModeHelper {
	private:

	public:
		virtual void		check_state();
		virtual void		gate_open(char team);
		virtual bool		player_shot(ServerPlayer& shooter, ServerPlayer& shot_player);
		virtual uint64_t	player_died(ServerPlayer& killer, ServerPlayer& killed);
		virtual void		game_timeout();
		virtual void		new_game();

		virtual bool		is_team_play() const { return false; }
		virtual GameMode	get_type() const { return DEATHMATCH; }

		explicit DeathmatchMode (Server& server) : GameModeHelper(server) { }

	};
}

#endif
