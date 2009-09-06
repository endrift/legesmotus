/*
 * server/ZombieMode.cpp
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

#include "ZombieMode.hpp"
#include "Server.hpp"
#include "ServerPlayer.hpp"
#include <stdlib.h>

using namespace LM;
using namespace std;

void	ZombieMode::check_state() {
	if (m_server.nbr_players() >= 2) {
		if (m_server.get_team_count('A') == 0) {
			// No humans left - zombies win
			m_server.game_over('B');
			m_server.new_game();
		} else if (m_server.get_team_count('B') == 0) {
			// No zombies left - humans win
			m_server.game_over('A');
			m_server.new_game();
		}
	}
}

void	ZombieMode::gate_open(char team) {
}

bool	ZombieMode::player_shot(ServerPlayer& shooter, ServerPlayer& shot_player) {
	return m_server.m_params.friendly_fire || shooter.get_team() != shot_player.get_team();
}

uint64_t	ZombieMode::player_died(ServerPlayer* killer, ServerPlayer& killed) {
	if (!killer) {
		return m_server.m_params.freeze_time;
	}

	int		score_change = 0;
	uint64_t	freeze_time = 0;

	if (killer->get_team() == killed.get_team()) {
		// Killed your own kind (d'oh) - results in a freeze and -1 score penalty
		score_change = -1;
		freeze_time = m_server.m_params.freeze_time;
	} else if (killer->get_team() == 'A') {
		// Human killed a zombie - zombie freezes
		score_change = 1;
		freeze_time = m_server.m_params.freeze_time;
	} else if (killer->get_team() == 'B') {
		// Zombie killed a human - human becomes zombie
		score_change = 1;
		m_server.change_team(killed, 'B', false, false);
	}

	if (score_change) {
		m_server.change_score(*killer, score_change);
	}

	return freeze_time;
}

void	ZombieMode::game_timeout() {
	m_server.game_over('A'); // Humans win
	m_server.new_game();
}

void	ZombieMode::new_game() {
	size_t	player_index = rand() % m_server.nbr_players();
	bool	has_zombie = false;

	for (Server::PlayerMap::iterator it(m_server.m_players.begin()); it != m_server.m_players.end(); ++it) {
		if (!has_zombie && player_index-- == 0) {
			has_zombie = true;
			m_server.change_team(it->second, 'B', true, false);
		} else {
			m_server.change_team(it->second, 'A', true, false);
		}
	}
}

