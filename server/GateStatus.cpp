/*
 * server/GateStatus.cpp
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

#include "GateStatus.hpp"
#include "Server.hpp"
#include "common/timer.hpp"
#include <limits>
#include <algorithm>

using namespace LM;
using namespace std;

GateStatus::GateStatus(const Server& server) : m_server(server) {
	reset();
}

uint64_t GateStatus::get_open_time() const {
	return m_server.get_gate_open_time(m_players.size());
}

uint64_t GateStatus::get_close_time() const {
	return m_server.get_gate_close_time();
}

uint64_t GateStatus::get_stick_time() const {
	return m_server.get_gate_stick_time();
}

uint64_t GateStatus::next_update_time() const {
	// TODO: take into account time until gate actually changes, in case it might be less than GATE_UPDATE_FREQUENCY (probably not an issue, because GATE_UPDATE_FREQUENCY is so small anyways)
	if (get_nbr_players() == 0 && !is_closed()) {
		uint64_t	time_since_change = get_ticks() - m_change_time;
		if (time_since_change >= get_stick_time()) {
			return Server::GATE_UPDATE_FREQUENCY;
		} else {
			return get_stick_time() - time_since_change;
		}

	} else if (get_nbr_players() > 0 && !is_open()) {
		return Server::GATE_UPDATE_FREQUENCY;
	}

	// Gate is not moving.
	return numeric_limits<uint64_t>::max();
}

bool GateStatus::update() {
	if (get_nbr_players() == 0 && !is_closed()) {
		uint64_t	now = get_ticks();
		uint64_t	time_since_change = now - m_change_time;
		if (time_since_change >= get_stick_time()) {
			// Since now - m_change_time >= get_stick_time(), m_change_time + get_stick_time() <= now
			uint64_t time_since_update = now - max(m_update_time, m_change_time + get_stick_time());
			if (get_close_time() > 0) {
				m_progress = max(0.0, m_progress - time_since_update / double(get_close_time()));
			} else {
				m_progress = 0.0;
			}
			m_update_time = now;
			return true;
		}
	} else if (get_nbr_players() > 0 && !is_open()) {
		uint64_t	now = get_ticks();
		uint64_t	time_since_update = now - m_update_time;
		if (get_open_time() > 0) {
			m_progress = min(1.0, m_progress + time_since_update / double(get_open_time()));
		} else {
			m_progress = 1.0;
		}
		m_update_time = now;
		return true;
	}

	return false;
}

void	GateStatus::reset() {
	m_progress = 0.0;
	m_update_time = 0;
	m_change_time = 0;
	m_players.clear();
}

bool	GateStatus::set_engagement(bool is_now_engaged, uint32_t new_player_id) {
	if (is_now_engaged && !m_players.count(new_player_id)) {
		// Gate is being engaged by a player who is not already engaging it...

		update();
		m_players.insert(new_player_id);
		m_change_time = m_update_time = get_ticks();
		update();

		return true;

	} else if (!is_now_engaged && m_players.count(new_player_id)) {
		// Gate is being disengaged by a player who was previously engaging it...

		update();
		m_players.erase(new_player_id);
		m_change_time = m_update_time = get_ticks();
		update();

		return true;
	}

	return false;
}

