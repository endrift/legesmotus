/*
 * ServerScannerNetwork.cpp
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
#include "ServerScannerNetwork.hpp"
#include "ServerScanner.hpp"
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

uint32_t	ServerScannerNetwork::next_connection_id = 1;

ServerScannerNetwork::ServerScannerNetwork(ServerScanner& controller) : m_controller(controller) {
	m_is_connected = false;
	m_last_packet_time = 0;
}

bool	ServerScannerNetwork::connect(const char* hostname, unsigned int portno) {
	disconnect();

	if (!resolve_hostname(m_server_address, hostname, portno)) {
		return false;
	}

	m_ack_manager.clear();
	m_is_connected = true;
	m_last_packet_time = 0;
	return true;
}

bool	ServerScannerNetwork::connect(const IPAddress& address) {
	disconnect();
	m_ack_manager.clear();
	m_server_address = address;
	m_is_connected = true;
	m_last_packet_time = 0;
	return true;
}

void	ServerScannerNetwork::disconnect() {
	m_is_connected = false;
	m_server_peer.init(next_connection_id++);
	m_server_peer.packet_queue.set_max_size(1024);
}


void	ServerScannerNetwork::send_reliable_packet(const PacketWriter& packet) {
	if (!is_connected()) {
		return;
	}

	PacketHeader	header(packet.packet_type(), m_server_peer.next_sequence_no++, m_server_peer.connection_id);
	send_packet_to(m_server_address, header, packet.packet_data());
	m_ack_manager.add_packet(m_server_address, header, packet.packet_data());
}


void	ServerScannerNetwork::send_packet(const PacketHeader& packet_header, const std::string& packet_data) {
	if (is_connected()) {
		send_packet_to(m_server_address, packet_header, packet_data);
	}
}


void	ServerScannerNetwork::broadcast_packet(unsigned int portno, const PacketWriter& packet) {
	send_packet_to(IPAddress(htonl(INADDR_BROADCAST), htons(portno)), packet);
}

void	ServerScannerNetwork::receive_packets(uint32_t timeout) {
	UDPPacket	raw_packet(MAX_PACKET_LENGTH);
	// Block until packets are received, timeout has elapsed, or a signal has been received.
	bool		has_packets = m_socket.has_packets(timeout);
	// Keep receiving packets for as long as we can.
	while (receive_raw_packet(raw_packet)) {
		process_unbound_packet(raw_packet);
	}
}

void	ServerScannerNetwork::process_unbound_packet(const UDPPacket& raw_packet) {
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

uint64_t ServerScannerNetwork::get_last_packet_time() {
	return m_last_packet_time;
}

