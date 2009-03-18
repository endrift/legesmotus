/*
 * server/ServerNetwork.cpp
 *
 * Copyright 2009 - Nathan Partlan, Andrew Ayer, Daniel Schneider, and Jeffrey Pfau
 * 
 */

#include "ServerNetwork.hpp"
#include "Server.hpp"
#include "common/LMException.hpp"
#include "common/network.hpp"
#include "common/PacketWriter.hpp"
#include "common/PacketReader.hpp"
#include "common/RawPacket.hpp"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

ServerNetwork::ServerNetwork() {
	m_socket = NULL;
	m_socket_set = SDLNet_AllocSocketSet(1);
	if (m_socket_set == NULL) {
		// Woah, nelly - should never happen
		throw LMException(SDLNet_GetError());
	}
}

ServerNetwork::~ServerNetwork() {
	stop();
	SDLNet_FreeSocketSet(m_socket_set);
}

bool	ServerNetwork::start(unsigned int portno) {
	stop();

	m_socket = SDLNet_UDP_Open(portno);

	if (m_socket == NULL) {
		return false;
	}

	SDLNet_UDP_AddSocket(m_socket_set, m_socket);

	m_next_unbound_channel = 1;

	return true;
}

void	ServerNetwork::stop() {
	if (is_running()) {
		SDLNet_UDP_DelSocket(m_socket_set, m_socket);
		SDLNet_UDP_Close(m_socket);
		m_socket = NULL;
		m_bound_channels.clear();
	}
}

void	ServerNetwork::send_packet(int channel, const PacketWriter& packet) {
	RawPacket	raw_packet(MAX_PACKET_LENGTH);

	raw_packet.fill(packet);

	//std::cerr << "Sending ___" << packet.packet_data() << "___ to " << channel << '\n';
	SDLNet_UDP_Send(m_socket, channel, raw_packet);
}

void	ServerNetwork::broadcast_packet(const PacketWriter& packet, int exclude_channel) {
	std::set<int>::const_iterator it(m_bound_channels.begin());
	while (it != m_bound_channels.end()) {
		if (*it != exclude_channel) {
			send_packet(*it, packet);
		}
		++it;
	}
}

bool	ServerNetwork::receive_packets(Server& server, long timeout) {
	int result = SDLNet_CheckSockets(m_socket_set, timeout);

	if (result <= 0) {
		// No sockets ready - timeout must have elapsed (or there was an error!)
		return false;
	}

	RawPacket	raw_packet(MAX_PACKET_LENGTH);

	// Receive all the packets we can.
	while (SDLNet_UDP_Recv(m_socket, raw_packet) == 1) {
		process_packet(server, raw_packet);
	}
	return true;
}

void	ServerNetwork::process_packet(Server& server, const RawPacket& raw_packet) {
	PacketReader	reader(raw_packet);
	int		channel = raw_packet->channel;

	if (channel == -1 && reader.packet_type() == JOIN_PACKET) {
		// Bind this address and give it a channel
		channel = SDLNet_UDP_Bind(m_socket, m_next_unbound_channel++, const_cast<IPaddress*>(&raw_packet->address));
		m_bound_channels.insert(channel);
	} else if (channel == -1) {
		// Ignore this wild packet
		return;
	}

	switch (reader.packet_type()) {
	case ACK_PACKET:
		//server.ack(channel, reader);
		break;

	case PLAYER_UPDATE_PACKET:
		server.player_update(channel, reader);
		break;

	case GUN_FIRED_PACKET:
		//server.gun_fired(channel, reader);
		break;

	case PLAYER_SHOT_PACKET:
		//server.player_shot(channel, reader);
		break;

	case MESSAGE_PACKET:
		//server.message(channel, reader);
		break;

	case GATE_DOWN_PACKET:
		//server.gate_down(channel, reader);
		break;

	case JOIN_PACKET:
		server.join(channel, reader);
		break;

	case SCORE_REQUEST_PACKET:
		//server.score_request(channel, reader);
		break;

	case LEAVE_PACKET:
		server.leave(channel, reader);
		break;
	}
}

void	ServerNetwork::unbind(int channel) {
	SDLNet_UDP_Unbind(m_socket, channel);
	m_bound_channels.erase(channel);
}

