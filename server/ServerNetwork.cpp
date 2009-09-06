/*
 * server/ServerNetwork.cpp
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

#include "ServerNetwork.hpp"
#include "Server.hpp"
#include "common/Exception.hpp"
#include "common/network.hpp"
#include "common/PacketWriter.hpp"
#include "common/PacketReader.hpp"
#include "common/UDPPacket.hpp"
#include "common/IPAddress.hpp"
#include <stdio.h>
#include <stdlib.h>
#ifndef __WIN32
#include <signal.h>
#endif

using namespace LM;
using namespace std;

ServerNetwork::ServerNetwork() {
}

ServerNetwork::~ServerNetwork() {
}

bool	ServerNetwork::start(const IPAddress& bind_address) {
	// TODO: check to make sure server isn't listening already

	return m_socket.bind(bind_address);
}

void	ServerNetwork::send_packet(const IPAddress& address, const PacketWriter& packet_data) {
	UDPPacket	raw_packet(MAX_PACKET_LENGTH);
	raw_packet.set_address(address);
	raw_packet.fill(packet_data);
	send_raw_packet(raw_packet);
}

void	ServerNetwork::send_packet(const IPAddress& address, const std::string& packet_data) {
	UDPPacket	raw_packet(MAX_PACKET_LENGTH);
	raw_packet.set_address(address);
	raw_packet.fill(packet_data);
	send_raw_packet(raw_packet);
}

void	ServerNetwork::send_raw_packet(const UDPPacket& raw_packet) {
	m_socket.send(raw_packet);
}

bool	ServerNetwork::receive_packets(Server& server, uint32_t timeout) {
#ifndef __WIN32
	// Now is an ideal time to handle signals, so unblock all signals
	sigset_t		old_sigset;
	sigset_t		new_sigset;
	sigemptyset(&new_sigset);
	sigprocmask(SIG_SETMASK, &new_sigset, &old_sigset);
#endif

	// Block until packets are received, timeout has elapsed, or a signal has been received.
	bool		has_packets = m_socket.has_packets(timeout);

#ifndef __WIN32
	// Restore the old signal mask
	sigprocmask(SIG_SETMASK, &old_sigset, NULL);
#endif

	if (!has_packets) {
		// Socket does not have packets to receive - timeout must have elapsed, or a signal was received
		return false;
	}

	UDPPacket	raw_packet(MAX_PACKET_LENGTH);

	// Receive all the packets we can.
	while (m_socket.has_packets() && m_socket.recv(raw_packet)) {
		process_packet(server, raw_packet);
	}
	return true;
}

void	ServerNetwork::process_packet(Server& server, const UDPPacket& raw_packet) {
	const IPAddress&	address = raw_packet.get_address();
	PacketReader		reader(raw_packet);

	switch (reader.packet_type()) {
	case ACK_PACKET:
		server.ack(address, reader);
		break;

	case PLAYER_UPDATE_PACKET:
		server.player_update(address, reader);
		break;

	case WEAPON_DISCHARGED_PACKET:
		server.weapon_discharged(address, reader);
		break;

	case PLAYER_HIT_PACKET:
		server.player_hit(address, reader);
		break;

	case MESSAGE_PACKET:
		server.message(address, reader);
		break;

	case GATE_UPDATE_PACKET:
		server.gate_update(address, reader);
		break;

	case JOIN_PACKET:
		server.join(address, reader);
		break;

	case INFO_PACKET:
		server.info(address, reader);
		break;

	case LEAVE_PACKET:
		server.leave(address, reader);
		break;

	case PLAYER_ANIMATION_PACKET:
		server.player_animation(address, reader);
		break;

	case NAME_CHANGE_PACKET:
		server.name_change(address, reader);
		break;

	case TEAM_CHANGE_PACKET:
		server.team_change(address, reader);
		break;

	case REGISTER_SERVER_PACKET:
		server.register_server_packet(address, reader);
		break;

	case MAP_INFO_PACKET:
		server.map_info_packet(address, reader);
		break;

	case HOLE_PUNCH_PACKET:
		server.hole_punch_packet(address, reader);
		break;

	case PLAYER_DIED_PACKET:
		server.player_died(address, reader);
		break;
	}
}

