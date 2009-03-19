/*
 * server/Server.cpp
 *
 * Copyright 2009 - Nathan Partlan, Andrew Ayer, Daniel Schneider, and Jeffrey Pfau
 * 
 */

#include "Server.hpp"
#include "ServerNetwork.hpp"
#include "common/LMException.hpp"
#include "common/PacketWriter.hpp"
#include "common/PacketReader.hpp"
#include "common/network.hpp"
#include <string>
#include <iostream>

using namespace std;

const int	Server::SERVER_VERSION = 1;

Server::Server ()
{
	m_next_player_id = 1;
}

void	Server::player_update(int channel, PacketReader& packet)
{
	// Just broadcast this packet to all other players
	PacketWriter		resent_packet(PLAYER_UPDATE_PACKET);
	resent_packet << packet;
	m_network.broadcast_packet(resent_packet, channel);
}

void	Server::join(int channel, PacketReader& packet)
{
	// XXX: Prototype function ONLY!  Needs to change!
	int			client_version;
	string			name;
	char			team;

	packet >> client_version >> name >> team;

	// TODO: check client version.

	if (team != 'A' && team != 'B') {
		// TODO: Assign to team equitably.
		team = 'A';
	}

	uint32_t		player_id = m_next_player_id++;

	m_players[player_id].init(player_id, channel, client_version, name.c_str(), team);

	// Send the welcome packet back to this client.
	PacketWriter		welcome_packet(WELCOME_PACKET);
	welcome_packet << SERVER_VERSION << player_id << team;
	m_network.send_packet(channel, welcome_packet);

	// Broadcast the announce packet back to all players, except for the new one
	PacketWriter		announce_packet(ANNOUNCE_PACKET);
	announce_packet << player_id << name << team;
	m_network.broadcast_packet(announce_packet, channel);

	// Send the new player an announce packet for every player currently in the game
	player_map::const_iterator	it(m_players.begin());
	while (it != m_players.end()) {
		const ServerPlayer&	player((it++)->second);

		PacketWriter	announce_packet(ANNOUNCE_PACKET);
		announce_packet << player.get_id() << player.get_name() << player.get_team();
		m_network.send_packet(channel, announce_packet);
	}
}

void	Server::leave(int channel, PacketReader& packet)
{
	uint32_t	player_id;
	packet >> player_id;

	m_network.unbind(channel);

	if (is_authorized(channel, player_id)) {
		m_players.erase(player_id);

		// Broadcast to the game that this player has left
		PacketWriter	leave_packet(LEAVE_PACKET);
		leave_packet << player_id;
		m_network.broadcast_packet(leave_packet);
	}
}

void	Server::run(int portno) // XXX: Prototype function ONLY!
{
	if (!m_network.start(portno)) {
		throw LMException("Failed to start server network on port.");
	}

	while (true) {
		m_network.receive_packets(*this, -1);
	}
}

// Arguments:
//  - channel: the channel that the packet is coming from
//  - player_id: the player ID which the packet claims to represent
bool	Server::is_authorized(int channel, uint32_t player_id) const {
	// Look up the player ID in the players map
	player_map::const_iterator	it(m_players.find(player_id));
	
	// Make sure that both:
	//  1. The alleged player actually exists, and
	//  2. The player's stored channel matches the channel that the request is coming from
	return it != m_players.end() && it->second.get_channel() == channel;
}

