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
		double			m_progress;	// [0..1] (0==fully closed, 1==fully open)
		uint64_t		m_update_time;	// The tick time at which the gate was last updated
		uint64_t		m_change_time;	// The tick time at which the gate was last changed (i.e. player engaged or disengaged)
		std::set<uint32_t>	m_players;	// The players who are engaging the gate

		uint64_t		get_open_time() const;
		uint64_t		get_close_time() const;
		uint64_t		get_stick_time() const;

	public:
		explicit GateStatus(const Server& server);

		// Get basic information about the gate:
		bool		is_open () const { return m_progress == 1.0; }
		bool		is_closed () const { return m_progress == 0.0; }
		size_t		get_nbr_players() const { return m_players.size(); }

		uint64_t	next_update_time() const;	// The maximum number of ticks until the gate MUST be updated again

		// Return a number in range [0,1] to indicate progress of gate:
		//  (0.0 == fully closed, 1.0 == fully open)
		double		get_progress() const { return m_progress; }

		// Update the progress of the gate based on how much time has elapsed:
		//  Returns true if the gate changed (meaning a GATE_UPDATE packet should be sent)
		//  Returns false if nothing changed about the gate
		//  Use next_update_time() to determine how often update() must be called
		bool		update();

		// Reset the gate to fully closed:
		//  Call at beginning of new games to fully reset the gate
		void		reset();

		// Engage or disengage the gate for the given player:
		//  Returns true if the gate state changed, false otherwise
		bool		set_engagement(bool is_engaged, uint32_t player_id);
	};
}

#endif
