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

	int			player_id = m_next_player_id++;

	// Send the welcome packet back to this client.
	PacketWriter		welcome_packet(WELCOME_PACKET);
	welcome_packet << client_version << player_id << team; // TODO: send back server version #
	m_network.send_packet(channel, welcome_packet);

	// Broadcast the announce packet back to all players
	PacketWriter		announce_packet(ANNOUNCE_PACKET);
	announce_packet << player_id << name << team;
	m_network.broadcast_packet(announce_packet);

	// TODO: actually save this player somewhere
}

void	Server::leave(int channel, PacketReader& packet)
{
	m_network.unbind(channel);
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



