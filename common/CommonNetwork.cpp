/*
 * common/CommonNetwork.cpp
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

#include "CommonNetwork.hpp"
#include "UDPPacket.hpp"
#include "IPAddress.hpp"
#include "PacketReader.hpp"
#include "PacketWriter.hpp"
#include "network.hpp"
#include "Packet.hpp"
#include <stdint.h>
#include <stdlib.h>

using namespace LM;
using namespace std;

CommonNetwork::Peer::Peer ()
{
	next_sequence_no = 1L;
}

void	CommonNetwork::Peer::init (uint32_t arg_connection_id, uint64_t next_send_sequence_no, uint64_t next_receive_sequence_no)
{
	connection_id = arg_connection_id;
	next_sequence_no = next_send_sequence_no;
	packet_queue.init(next_receive_sequence_no);
}

void	CommonNetwork::send_raw_packet(const UDPPacket& raw_packet) {
/*
	static UDPPacket*	buffered_packet = NULL;
	static long		packet_count = 0;

	++packet_count;

	if (packet_count > 30 && rand() % 3 == 0) {
		return;
	}

	if (packet_count > 30) {
		if (buffered_packet) {
			m_socket.send(raw_packet);
			m_socket.send(*buffered_packet);
			delete buffered_packet;
			buffered_packet = NULL;
		} else if (rand() % 3 == 0) {
			buffered_packet = new UDPPacket(raw_packet);
		}
	} else {
		m_socket.send(raw_packet);
	}
	*/

	m_socket.send(raw_packet);
	//m_socket.send(raw_packet);
	//m_socket.send(raw_packet);
}

bool	CommonNetwork::receive_raw_packet(UDPPacket& raw_packet) {
	return m_socket.has_packets() && m_socket.recv(raw_packet);
}

void	CommonNetwork::send_ack(const IPAddress& peer, const PacketReader& packet_to_ack) {
	PacketWriter		ack_packet(ACK_PACKET);
	ack_packet << packet_to_ack.packet_type() << packet_to_ack.sequence_no();
	send_packet(peer, ack_packet);
}

void	CommonNetwork::send_ack(const IPAddress& peer, const Packet& packet_to_ack) {
	Packet ack_packet(ACK_PACKET);
	ack_packet.ack.packet_type = packet_to_ack.header.packet_type;
	ack_packet.ack.sequence_no = packet_to_ack.header.sequence_no;
	send_packet(peer, &ack_packet);
}

void	CommonNetwork::process_ack(const IPAddress& peer, PacketReader& ack_packet) {
	uint32_t	packet_type;
	uint64_t	sequence_no;
	ack_packet >> packet_type >> sequence_no;

	m_ack_manager.ack(peer, sequence_no);
}

void	CommonNetwork::send_packet(const IPAddress& dest, Packet* packet) {
	packet->marshal();
	send_raw_packet(packet->raw);
}

void	CommonNetwork::send_packet(const IPAddress& dest, const PacketWriter& packet) {
	send_packet(dest, packet.get_header(), packet.packet_data());
}

void	CommonNetwork::send_packet(const IPAddress& dest, const PacketHeader& packet_header, const std::string& packet_data) {
	UDPPacket	raw_packet(MAX_PACKET_LENGTH);
	raw_packet.set_address(dest);
	raw_packet.fill(packet_header.make_string());
	raw_packet.append(packet_data);
	send_raw_packet(raw_packet);
}

void	CommonNetwork::resend_acks() {
	m_ack_manager.resend(*this);
}

