/*
 * common/network.hpp
 *
 * Copyright 2009 - Nathan Partlan, Andrew Ayer, Daniel Schneider, and Jeffrey Pfau
 * 
 */

#ifndef LM_COMMON_NETWORK_HPP
#define LM_COMMON_NETWORK_HPP

// What to use to separate packet fields?
//const char PACKET_FIELD_SEPARATOR = '\f';	// Formfeed
const char PACKET_FIELD_SEPARATOR = '~';	// Formfeed

// Maximum length of packets:
enum { MAX_PACKET_LENGTH = 1024 };

// The default port number of the server
enum { DEFAULT_PORTNO = 9009 };

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
	PLAYER_ANIMATION_PACKET = 15
};

#endif
