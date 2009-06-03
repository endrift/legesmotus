/*
 * ClientNetwork.cpp
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
 
#include <string>
#include "ClientNetwork.hpp"
#include "GameController.hpp"
#include "common/network.hpp"
#include "common/PacketWriter.hpp"
#include "common/PacketReader.hpp"
#include "common/UDPPacket.hpp"
#include "common/UDPSocket.hpp"
#include "common/IPAddress.hpp"
#include <iostream>

using namespace std;

ClientNetwork::ClientNetwork() {
	m_is_connected = false;
}

ClientNetwork::~ClientNetwork() {
}

bool	ClientNetwork::connect(const char* hostname, unsigned int portno) {
	disconnect();

	if (!resolve_hostname(m_server_address, hostname, portno)) {
		return false;
	}

	m_is_connected = true;
	return true;
}

bool	ClientNetwork::connect(const IPAddress& address) {
	disconnect();
	m_server_address = address;
	m_is_connected = true;
	return true;
}

void	ClientNetwork::disconnect() {
	m_is_connected = false;
}

void	ClientNetwork::send_packet(const PacketWriter& packet) {
	if (is_connected()) {
		send_packet_to(m_server_address, packet);
	}
}

void	ClientNetwork::send_packet_to(const IPAddress& dest, const PacketWriter& packet) {
	UDPPacket	raw_packet(MAX_PACKET_LENGTH);
	raw_packet.set_address(dest);
	raw_packet.fill(packet);
	send_raw_packet(raw_packet);
}

void	ClientNetwork::broadcast_packet(unsigned int portno, const PacketWriter& packet) {
	UDPPacket	raw_packet(MAX_PACKET_LENGTH);
	raw_packet.set_address(IPAddress(htonl(INADDR_BROADCAST), htons(portno))); // TODO: abstract the INADDR_BROADCAST and the htonl/htons
	raw_packet.fill(packet);
	send_raw_packet(raw_packet);
}

void	ClientNetwork::send_raw_packet(const UDPPacket& raw_packet) {
	m_socket.send(raw_packet);
}

bool	ClientNetwork::receive_raw_packet(UDPPacket& raw_packet) {
	return m_socket.has_packets() && m_socket.recv(raw_packet);
}

void	ClientNetwork::receive_packets(GameController& controller) {
	UDPPacket	raw_packet(MAX_PACKET_LENGTH);

	// Keep receiving packets for as long as we can.
	while (receive_raw_packet(raw_packet)) {
		if (is_connected() && raw_packet.get_address() == m_server_address) {
			process_server_packet(controller, raw_packet);
		} else {
			process_unbound_packet(controller, raw_packet);
		}
	}
}

void	ClientNetwork::process_server_packet(GameController& controller, const UDPPacket& raw_packet) {
	PacketReader	reader(raw_packet);

	switch (reader.packet_type()) {
	case PLAYER_UPDATE_PACKET:
		controller.player_update(reader);
		break;

	case GUN_FIRED_PACKET:
		controller.gun_fired(reader);
		break;

	case PLAYER_SHOT_PACKET:
		//send_ack(reader.packet_id());
		controller.player_shot(reader);
		break;

	case MESSAGE_PACKET:
		controller.message(reader);
		break;

	case GAME_START_PACKET:
		//send_ack(reader.packet_id());
		controller.game_start(reader);
		break;

	case GAME_STOP_PACKET:
		//send_ack(reader.packet_id());
		controller.game_stop(reader);
		break;

	case SCORE_UPDATE_PACKET:
		controller.score_update(reader);
		break;

	case WELCOME_PACKET:
		//send_ack(reader.packet_id());
		controller.welcome(reader);
		break;

	case ANNOUNCE_PACKET:
		controller.announce(reader);
		break;

	case GATE_UPDATE_PACKET:
		controller.gate_update(reader);
		break;

	case SHUTDOWN_PACKET:
		//controller.shutdown(reader);
		break;

	case LEAVE_PACKET:
		controller.leave(reader);
		break;

	case PLAYER_ANIMATION_PACKET:
		controller.animation_packet(reader);
		break;

	case REQUEST_DENIED_PACKET:
		controller.request_denied(reader);
		break;

	case NAME_CHANGE_PACKET:
		controller.name_change(reader);
		break;

	case TEAM_CHANGE_PACKET:
		controller.team_change(reader);
		break;

	case INFO_PACKET:
		controller.server_info(raw_packet.get_address(), reader);
		break;
	}
}

void	ClientNetwork::process_unbound_packet(GameController& controller, const UDPPacket& raw_packet) {
	PacketReader	reader(raw_packet);
	
	switch (reader.packet_type()) {
	case INFO_PACKET:
		controller.server_info(raw_packet.get_address(), reader);
		break;
	}
}

