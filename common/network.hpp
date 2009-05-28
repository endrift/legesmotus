/*
 * common/network.hpp
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

#ifndef LM_COMMON_NETWORK_HPP
#define LM_COMMON_NETWORK_HPP

#include <string>
#include "IPAddress.hpp"

// What to use to separate packet fields?
const char PACKET_FIELD_SEPARATOR = '\f';	// Formfeed

// Maximum length of packets:
enum { MAX_PACKET_LENGTH = 1024 };

// Maximum length of a player name
enum { MAX_NAME_LENGTH = 20 };

// The default port number of the server
enum { DEFAULT_PORTNO = 16877 };

// Packet types:
enum {
	ACK_PACKET = 0,
	PLAYER_UPDATE_PACKET = 1,
	GUN_FIRED_PACKET = 2,
	PLAYER_SHOT_PACKET = 3,
	MESSAGE_PACKET = 4,
	GAME_START_PACKET = 5,
	GAME_STOP_PACKET = 6,
	SCORE_UPDATE_PACKET = 7,
	WELCOME_PACKET = 8,
	ANNOUNCE_PACKET = 9,
	GATE_UPDATE_PACKET = 10,
	JOIN_PACKET = 11,
	INFO_PACKET = 12,
	SHUTDOWN_PACKET = 13,
	LEAVE_PACKET = 14,
	PLAYER_ANIMATION_PACKET = 15,
	REQUEST_DENIED_PACKET = 16,
	NAME_CHANGE_PACKET = 17,
	TEAM_CHANGE_PACKET = 18
};

bool		resolve_hostname(IPAddress& resolved_addr, const char* hostname_to_resolve, uint16_t portno); // portno must be in host-byte order
bool		resolve_ip_address(std::string& resolved_hostname, uint16_t* portno, const IPAddress& address_to_resolve); // portno will be in host-byte order

std::string	format_ip_address(const IPAddress& addr, bool resolve =false);

#ifdef __WIN32
int		inet_aton(const char* cp, struct in_addr* inp); // Compatibility wrapper for Windows

typedef int socklen_t;
#endif

#endif
