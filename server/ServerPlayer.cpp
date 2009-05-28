/*
 * server/ServerPlayer.cpp
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

#include "ServerPlayer.hpp"
#include "Server.hpp"
#include "common/timer.hpp"

using namespace std;

ServerPlayer::ServerPlayer() {
	m_client_version = -1;
	m_is_op = false;
	m_spawnpoint = NULL;
	m_join_time = m_last_seen_time = 0;
}

ServerPlayer& ServerPlayer::init(uint32_t player_id, const IPAddress& address, int client_version, const char* name, char team, ServerPlayer::Queue& timeout_queue) {
	Player::set_id(player_id);
	Player::set_name(name);
	Player::set_team(team);

	m_address = address;
	m_client_version = client_version;

	m_join_time = m_last_seen_time = get_ticks();

	timeout_queue.push_back(this);
	m_timeout_queue_position = --timeout_queue.end();

	return *this;
}

void ServerPlayer::reset_join_time() {
	m_join_time = get_ticks();
}

uint64_t ServerPlayer::time_until_spawn() const {
	uint64_t time_since_join = get_ticks() - m_join_time;
	return time_since_join >= Server::JOIN_DELAY ? 0 : Server::JOIN_DELAY - time_since_join;
}

void ServerPlayer::seen(Queue& timeout_queue) {
	m_last_seen_time = get_ticks();

	timeout_queue.erase(m_timeout_queue_position);
	timeout_queue.push_back(this);
	m_timeout_queue_position = --timeout_queue.end();
}

bool ServerPlayer::has_timed_out() const {
	return get_ticks() - m_last_seen_time >= Server::PLAYER_TIMEOUT;
}


