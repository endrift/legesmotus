/*
 * newclient/ClientNetwork.cpp
 *
 * This file is part of Leges Motus, a networked, 2D shooter set in zero gravity.
 * 
 * Copyright 2009-2010 Andrew Ayer, Nathan Partlan, Jeffrey Pfau
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

#include "ClientNetwork.hpp"
#include "common/misc.hpp"
#include "common/Packet.hpp"

using namespace LM;
using namespace std;

uint32_t ClientNetwork::next_connection_id = 1;

ClientNetwork::ClientNetwork(PacketReceiver* client) {
	m_client = client;
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


void	ClientNetwork::send_reliable_packet(Packet* packet) {
	if (!is_connected()) {
		return;
	}

	packet->header = PacketHeader(packet->type, m_server_peer.next_sequence_no++, m_server_peer.connection_id);
	send_packet_to(m_server_address, packet);
	m_ack_manager.add_packet(m_server_address, *packet);
}


void	ClientNetwork::send_packet(Packet* packet) {
	if (is_connected()) {
		packet->header.sequence_no = 0;
		packet->header.connection_id = 0;
		send_packet_to(m_server_address, packet);
	}
}


void	ClientNetwork::broadcast_packet(unsigned int portno, Packet* packet) {
	send_packet_to(IPAddress(htonl(INADDR_BROADCAST), htons(portno)), packet);
}

void	ClientNetwork::receive_packets() {
	Packet packet;

	// Keep receiving packets for as long as we can.
	while (receive_raw_packet(packet.raw)) {
		if (is_connected() && packet.raw.get_address() == m_server_address) {
			packet.unmarshal();
			if (packet.type != PLAYER_UPDATE_PACKET) {
				// Too many packets will get alerted if we leave this for PLAYER_UPDATE
				DEBUG("Received packet of type " << packet.type);
			}
			if (packet.header.sequence_no) {
				try {
					// High reliability packet
					// Immediately send an ACK
					send_ack(packet.raw.get_address(), packet);
					if (packet.header.connection_id == m_server_peer.connection_id && m_server_peer.packet_queue.push(packet)) {
						// Ready to be processed now.
						packet.dispatch(m_client);

						// Process any other packets that might be waiting in the queue.
						while (m_server_peer.packet_queue.has_packet()) {
							m_server_peer.packet_queue.peek().dispatch(m_client);
							m_server_peer.packet_queue.pop();
						}
					}
				} catch (PacketQueue::FullQueueException) {
					excessive_packet_drop(m_server_address);
				}
			} else {
				// Low reliability packet - we don't care if, when, or how often it arrives
				if (packet.type == ACK_PACKET) {
					process_ack(packet);
				} else {
					packet.dispatch(m_client);
				}
			}
		} else {
			// TODO handle unbound dispatches differently from bound ones
			packet.dispatch(m_client);
		}
	}
}

uint64_t ClientNetwork::get_last_packet_time() {
	return m_last_packet_time;
}

void	ClientNetwork::excessive_packet_drop(const IPAddress& peer) {
	if (m_is_connected && peer == m_server_address) {
		// TODO
		//m_controller.excessive_packet_drop();
	}
}
