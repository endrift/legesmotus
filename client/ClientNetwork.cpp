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
#include "common/PacketHeader.hpp"
#include "common/UDPPacket.hpp"
#include "common/UDPSocket.hpp"
#include "common/IPAddress.hpp"
#include "common/timer.hpp"
#include <iostream>

using namespace LM;
using namespace std;

uint32_t	ClientNetwork::next_connection_id = 1;

ClientNetwork::ClientNetwork(GameController& controller) : m_controller(controller) {
	m_is_connected = false;
	m_last_packet_time = 0;
}

bool	ClientNetwork::connect(const char* hostname, unsigned int portno) {
	disconnect();

	if (!resolve_hostname(m_server_address, hostname, portno)) {
		return false;
	}

	m_ack_manager.clear();
	m_is_connected = true;
	m_last_packet_time = 0;
	return true;
}

bool	ClientNetwork::connect(const IPAddress& address) {
	disconnect();
	m_ack_manager.clear();
	m_server_address = address;
	m_is_connected = true;
	m_last_packet_time = 0;
	return true;
}

void	ClientNetwork::disconnect() {
	m_is_connected = false;
	m_server_peer.init(next_connection_id++);
	m_server_peer.packet_queue.set_max_size(1024);
}


void	ClientNetwork::send_reliable_packet(const PacketWriter& packet) {
	if (!is_connected()) {
		return;
	}

	PacketHeader	header(packet.packet_type(), m_server_peer.next_sequence_no++, m_server_peer.connection_id);
	send_packet_to(m_server_address, header, packet.packet_data());
	m_ack_manager.add_packet(m_server_address, header, packet.packet_data());
}


void	ClientNetwork::send_packet(const PacketHeader& packet_header, const std::string& packet_data) {
	if (is_connected()) {
		send_packet_to(m_server_address, packet_header, packet_data);
	}
}


void	ClientNetwork::broadcast_packet(unsigned int portno, const PacketWriter& packet) {
	send_packet_to(IPAddress(htonl(INADDR_BROADCAST), htons(portno)), packet);
}

void	ClientNetwork::receive_packets() {
	UDPPacket	raw_packet(MAX_PACKET_LENGTH);

	// Keep receiving packets for as long as we can.
	while (receive_raw_packet(raw_packet)) {
		if (is_connected() && raw_packet.get_address() == m_server_address) {
			PacketReader		packet(raw_packet);

			if (packet.sequence_no()) {
				try {
					// High reliability packet
					// Immediately send an ACK
					send_ack(raw_packet.get_address(), packet);
					if (packet.connection_id() == m_server_peer.connection_id && m_server_peer.packet_queue.push(packet)) {
						// Ready to be processed now.
						process_server_packet(packet);

						// Process any other packets that might be waiting in the queue.
						while (m_server_peer.packet_queue.has_packet()) {
							process_server_packet(m_server_peer.packet_queue.peek());
							m_server_peer.packet_queue.pop();
						}
					}
				} catch (PacketQueue::FullQueueException) {
					m_controller.excessive_packet_drop();
				}
			} else {
				// Low reliability packet - we don't care if, when, or how often it arrives
				process_server_packet(packet);
			}
		} else {
			process_unbound_packet(raw_packet);
		}
	}
}

void	ClientNetwork::process_server_packet(PacketReader& reader) {
	m_last_packet_time = get_ticks();

	switch (reader.packet_type()) {
	case ACK_PACKET:
		process_ack(m_server_address, reader);
		break;

	case PLAYER_UPDATE_PACKET:
		m_controller.player_update(reader);
		break;

	case WEAPON_DISCHARGED_PACKET:
		m_controller.weapon_discharged(reader);
		break;

	case PLAYER_HIT_PACKET:
		m_controller.player_hit(reader);
		break;

	case MESSAGE_PACKET:
		m_controller.message(reader);
		break;

	case NEW_ROUND_PACKET:
		m_controller.new_round(reader);
		break;

	case ROUND_OVER_PACKET:
		m_controller.round_over(reader);
		break;

	case SCORE_UPDATE_PACKET:
		m_controller.score_update(reader);
		break;

	case WELCOME_PACKET:
		m_controller.welcome(reader);
		break;

	case ANNOUNCE_PACKET:
		m_controller.announce(reader);
		break;

	case GATE_UPDATE_PACKET:
		m_controller.gate_update(reader);
		break;

	case LEAVE_PACKET:
		m_controller.leave(reader);
		break;

	case PLAYER_ANIMATION_PACKET:
		m_controller.animation_packet(reader);
		break;

	case REQUEST_DENIED_PACKET:
		m_controller.request_denied(reader);
		break;

	case NAME_CHANGE_PACKET:
		m_controller.name_change(reader);
		break;

	case TEAM_CHANGE_PACKET:
		m_controller.team_change(reader);
		break;


	case MAP_INFO_PACKET:
		m_controller.map_info_packet(reader);
		break;

	case MAP_OBJECT_PACKET:
		m_controller.map_object_packet(reader);
		break;

	case GAME_PARAM_PACKET:
		m_controller.game_param_packet(reader);
		break;

	case INFO_PACKET:
		m_controller.server_info(m_server_address, reader);
		break;

	case PLAYER_DIED_PACKET:
		m_controller.player_died(reader);
		break;

	case WEAPON_INFO_PACKET:
		m_controller.weapon_info_packet(reader);
		break;

	case ROUND_START_PACKET:
		m_controller.round_start(reader);
		break;

	case SPAWN_PACKET:
		m_controller.spawn_packet(reader);
		break;
	}
}

void	ClientNetwork::process_unbound_packet(const UDPPacket& raw_packet) {
	PacketReader	reader(raw_packet);
	
	switch (reader.packet_type()) {
	case INFO_PACKET:
		m_controller.server_info(raw_packet.get_address(), reader);
		break;
	#ifndef LM_NO_UPGRADE_NAG
	case UPGRADE_AVAILABLE_PACKET:
		m_controller.upgrade_available(raw_packet.get_address(), reader);
		break;
	#endif
	case HOLE_PUNCH_PACKET:
		m_controller.hole_punch_packet(raw_packet.get_address(), reader);
		break;
	}
}

uint64_t ClientNetwork::get_last_packet_time() {
	return m_last_packet_time;
}

void	ClientNetwork::excessive_packet_drop(const IPAddress& peer) {
	if (m_is_connected && peer == m_server_address) {
		m_controller.excessive_packet_drop();
	}
}

