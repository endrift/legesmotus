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

uint64_t GateStatus::time_elapsed() const {
	return is_moving() ? get_ticks() - m_start_time : 0;
}

uint64_t GateStatus::time_remaining() const {
	if (is_moving()) {
		uint64_t	elapsed_time = time_elapsed();
		if (m_status == OPENING && elapsed_time < get_open_time()) {
			return get_open_time() - elapsed_time;
		} else if (m_status == CLOSING && elapsed_time < get_close_time()) {
			return get_close_time() - elapsed_time;
		}

		// Gate has finished moving.
		return 0;
	} else {
		// Gate is not moving.
		return numeric_limits<uint64_t>::max();
	}
}

void GateStatus::update() {
	if (m_status == OPENING && time_elapsed() >= get_open_time()) {
		m_status = OPEN;
		m_start_time = 0;
	} else if (m_status == CLOSING && time_elapsed() >= get_close_time()) {
		m_status = CLOSED;
		m_start_time = 0;
	}
}

double	GateStatus::get_progress() const {
	switch (m_status) {
	case CLOSED:
		return 0.0;
	case OPEN:
		return 1.0;
	case OPENING:
		return get_open_time() > 0 ? time_elapsed() / double(get_open_time()) : 1.0;
	case CLOSING:
		return get_close_time() > 0 ? 1.0 - time_elapsed() / double(get_close_time()) : 0.0;
	}
	return 0.0;
}

void	GateStatus::reset() {
	m_status = CLOSED;
	m_players.clear();
	m_start_time = 0;
}

void	GateStatus::set_progress(double progress) {
	if (m_status == OPENING) {
		m_start_time = get_ticks() - uint64_t(progress * get_open_time());
	} else if (m_status == CLOSING) {
		m_start_time = get_ticks() - uint64_t((1.0 - progress) * get_close_time());
	}
}

bool	GateStatus::set_engagement(bool is_now_engaged, uint32_t new_player_id) {
	bool	gate_was_changed = false;

	if (is_now_engaged) {
		// Gate is being engaged...
		double	old_progress = get_progress();
		if (!is_engaged()) {
			// It wasn't already engaged, so start opening it...
			m_status = OPENING;
			gate_was_changed = true;
		}

		m_players.insert(new_player_id);
		set_progress(old_progress);

	} else if (!is_now_engaged && is_engaged() && m_players.count(new_player_id)) {
		// Gate is being disengaged by a player who was previously engaging it...
		double	old_progress = get_progress();
		m_players.erase(new_player_id);

		if (m_players.empty()) {
			// No players left engaging the gate, so start closing it...
			m_status = CLOSING;
			gate_was_changed = true;
		}
		set_progress(old_progress);
	}

	return gate_was_changed;
}

