/*
 * server/GateStatus.hpp
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

#ifndef LM_SERVER_GATESTATUS_HPP
#define LM_SERVER_GATESTATUS_HPP

#include <set>
#include <stddef.h>
#include <stdint.h>

namespace LM {
	class Server;

	// Keeps track of the info for a team's gate.  Only used internally by Server.
	class GateStatus {
	public:
		enum {
			CLOSED = 0,
			OPENING = 1,
			OPEN = 2,
			CLOSING = 3
		};
	private:
		const Server&		m_server;	// The server that this gate is part of
		int			m_status;	// CLOSED, OPENING, or CLOSING
		std::set<uint32_t>	m_players;	// The players who are engaging the gate
		uint64_t		m_start_time;	// The tick time at which the gate started to be moved

		void			set_progress(double progress);
		uint64_t		get_open_time() const;
		uint64_t		get_close_time() const;

	public:
		explicit GateStatus(const Server& server);

		// Get basic information about the gate:
		bool		is_moving() const { return m_status == OPENING || m_status == CLOSING; }
		bool		is_engaged() const { return m_status == OPENING || m_status == OPEN; }
		int		get_status() const { return m_status; }
		size_t		get_nbr_players() const { return m_players.size(); }
		// TODO: which player ID should I return above?  Right now this information isn't being used (it's sent to the client, which ignores it), but if the client ever provides information about who is lowering the gate, this would be important.

		uint64_t	time_elapsed() const;	// If moving, how many milliseconds since the gate started moving?
		uint64_t	time_remaining() const;	// If moving, how many milliseconds until the gate finishes moving?

		// Return a number in range [0,1] to indicate progress of gate:
		//  (0.0 == fully closed, 1.0 == fully open)
		double		get_progress() const;

		// Update the status of the gate based on how much time has elapsed:
		//  Call every GATE_UPDATE_FREQUENCY milliseconds when gate is moving
		void		update();

		// Reset the gate to fully closed:
		//  Call at beginning of new games to fully reset the gate
		void		reset();

		// Engage or disengage the gate for the given player:
		//  Returns true if the gate state changed, false otherwise
		bool		set_engagement(bool is_engaged, uint32_t player_id);
	};
}

#endif
