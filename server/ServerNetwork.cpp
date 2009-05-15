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

	m_unbound_channels.clear();
	for (int i = 0; i < SDLNET_MAX_UDPCHANNELS; ++i) {
		m_unbound_channels.push_back(i);
	}

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

void	ServerNetwork::send_packet(int channel, const std::string& packet_data) {
	RawPacket	raw_packet(MAX_PACKET_LENGTH);
	raw_packet.fill(packet_data);
	send_raw_packet(channel, raw_packet);
}

void	ServerNetwork::send_packet(int channel, const PacketWriter& packet) {
	RawPacket	raw_packet(MAX_PACKET_LENGTH);
	raw_packet.fill(packet);
	send_raw_packet(channel, raw_packet);
}

void	ServerNetwork::send_packet(const IPaddress& addr, const PacketWriter& packet) {
	RawPacket	raw_packet(MAX_PACKET_LENGTH);
	raw_packet.set_address(addr);
	raw_packet.fill(packet);
	send_raw_packet(-1, raw_packet);
}

void	ServerNetwork::send_raw_packet(int channel, RawPacket& raw_packet) {
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

bool	ServerNetwork::receive_packets(Server& server, uint32_t timeout) {
	if (timeout == 0) {
		// Immediately timeout.
		return false;
	}

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
	if (raw_packet->channel == -1) {
		process_unbound_packet(server, raw_packet);
	} else {
		process_bound_packet(server, raw_packet);
	}
}

void	ServerNetwork::process_unbound_packet(Server& server, const RawPacket& raw_packet) {
	const IPaddress&	address = raw_packet->address;
	PacketReader		reader(raw_packet);

	switch (reader.packet_type()) {
	case JOIN_PACKET:
		server.join(address, reader);
		break;

	case INFO_PACKET:
		server.info(address, reader);
		break;

	}
}

void	ServerNetwork::process_bound_packet(Server& server, const RawPacket& raw_packet) {
	int		channel = raw_packet->channel;
	PacketReader	reader(raw_packet);

	switch (reader.packet_type()) {
	case ACK_PACKET:
		server.ack(channel, reader);
		break;

	case PLAYER_UPDATE_PACKET:
		server.player_update(channel, reader);
		break;

	case GUN_FIRED_PACKET:
		server.gun_fired(channel, reader);
		break;

	case PLAYER_SHOT_PACKET:
		server.player_shot(channel, reader);
		break;

	case MESSAGE_PACKET:
		server.message(channel, reader);
		break;

	case GATE_UPDATE_PACKET:
		server.gate_update(channel, reader);
		break;

	case LEAVE_PACKET:
		server.leave(channel, reader);
		break;

	case PLAYER_ANIMATION_PACKET:
		server.player_animation(channel, reader);
		break;

	case NAME_CHANGE_PACKET:
		server.name_change(channel, reader);
		break;

	case TEAM_CHANGE_PACKET:
		server.team_change(channel, reader);
		break;
	}
}

int	ServerNetwork::bind(const IPaddress& address) {
	if (m_unbound_channels.empty()) {
		// No channels available
		return -1;
	}

	int channel = SDLNet_UDP_Bind(m_socket, m_unbound_channels.front(), const_cast<IPaddress*>(&address));
	if (channel != -1) {
		m_unbound_channels.pop_front();
		m_bound_channels.insert(channel);
	}

	return channel;
}

void	ServerNetwork::unbind(int channel) {
	if (m_bound_channels.count(channel)) {
		SDLNet_UDP_Unbind(m_socket, channel);
		m_bound_channels.erase(channel);
		m_unbound_channels.push_front(channel);
	}
}

