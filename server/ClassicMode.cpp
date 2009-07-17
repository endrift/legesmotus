/*
 * server/ClassicMode.cpp
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

#include "ClassicMode.hpp"
#include "Server.hpp"
#include "ServerPlayer.hpp"
#include "common/team.hpp"

using namespace LM;
using namespace std;

void	ClassicMode::check_state() {
}

void	ClassicMode::gate_open(char team) {
	m_server.game_over(get_other_team(team));
	m_server.new_game();
}

uint64_t	ClassicMode::player_shot(ServerPlayer& shooter, ServerPlayer& shot_player) {
	int			score_change = 0;
	uint64_t		freeze_time = 0;

	if (!shot_player.is_frozen()) {
		if (shooter.get_team() != shot_player.get_team()) {
			// Shot an enemy
			// Results in a +1 score
			score_change = 1;
			freeze_time = m_server.m_params.freeze_time;
		} else if (m_server.m_params.friendly_fire) {
			// Shot a teammate and friendly fire is enabled!
			// Results in a -1 scoring penalty
			score_change = -1;
			freeze_time = m_server.m_params.freeze_time;
		}
	}

	if (score_change) {
		m_server.change_score(shooter, score_change);
	}

	return freeze_time;
}

void	ClassicMode::game_timeout() {
	m_server.game_over(0);
	m_server.new_game();
}

void	ClassicMode::new_game() {
}

