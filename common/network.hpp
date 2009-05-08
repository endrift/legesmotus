/*
 * common/network.hpp
 *
 * Copyright 2009 - Nathan Partlan, Andrew Ayer, Daniel Schneider, and Jeffrey Pfau
 * 
 */

#ifndef LM_COMMON_NETWORK_HPP
#define LM_COMMON_NETWORK_HPP

#include <string>
#include "SDL_net.h"

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
	SHUTDOWN_PACKET = 13,
	LEAVE_PACKET = 14,
	PLAYER_ANIMATION_PACKET = 15,
	REQUEST_DENIED_PACKET = 16,
	NAME_CHANGE_PACKET = 17,
	TEAM_CHANGE_PACKET = 18
};

std::string	format_ip_address(const IPaddress& addr);

#endif
