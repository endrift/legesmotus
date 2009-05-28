/*
 * server/ServerPlayer.hpp
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

#ifndef LM_SERVER_SERVERPLAYER_HPP
#define LM_SERVER_SERVERPLAYER_HPP

#include "common/Player.hpp"
#include "common/Point.hpp"
#include "common/IPAddress.hpp"
#include <stdint.h>
#include <list>

class ServerPlayer : public Player {
public:
	typedef std::list<ServerPlayer*> Queue;

private:
	IPAddress	m_address;		// The address from which the player is connecting.
	int		m_client_version;	// The version of the player's client.

	bool		m_is_op;		// This player has been authenticated with op status

	const Point*	m_spawnpoint;		// Where the player was spawned, if anywhere yet

	uint64_t	m_join_time;		// The tick time at which the player joined the game
	uint64_t	m_last_seen_time;	// The tick time at which this player was last seen (i.e. last had a packet from)

	// Iterator into a list which keeps track of when players were last seen:
	Queue::iterator	m_timeout_queue_position;

public:
	ServerPlayer();

	// Standard getters
	const IPAddress& get_address() const { return m_address; }
	int		get_client_version() const { return m_client_version; }

	bool		is_op() const { return m_is_op; }
	void		set_is_op(bool isop) { m_is_op = isop; }

	// Remembering spawn points
	bool		has_spawnpoint() const { return m_spawnpoint != NULL; }
	const Point*	get_spawnpoint() const { return m_spawnpoint; }
	void		set_spawnpoint(const Point* p) { m_spawnpoint = p; }

	// For spawning
	void		reset_join_time();
	uint64_t	time_until_spawn() const;	// How many milliseconds until this player can spawn?
	bool		is_ready_to_spawn() const { return time_until_spawn() == 0; }

	// For time out handling
	void		seen(Queue& timeout_queue);	// Update last seen time
	bool		has_timed_out() const;		// True if this player has timed out
	Queue::iterator	get_timeout_queue_position() const { return m_timeout_queue_position; }

	// Initialize the player
	ServerPlayer&	init(uint32_t player_id, const IPAddress& address, int client_version, const char* name, char team, Queue& timeout_queue);

};

#endif
