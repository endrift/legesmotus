/*
 * common/AckManager.cpp
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

#include "AckManager.hpp"
#include "PacketWriter.hpp"
#include "PacketWriter.hpp"
#include "CommonNetwork.hpp"
#include "timer.hpp"
#include <limits>
#include <iostream>

using namespace LM;
using namespace std;


AckManager::SentPacket::SentPacket(const IPAddress& peer_addr, const PacketHeader& arg_header, const string& arg_data) : packet_data(arg_data) {
	send_time = get_ticks();
	tries_left = RETRIES;
	add_recipient(peer_addr, arg_header);
}

AckManager::SentPacket::SentPacket(const string& arg_data) : packet_data(arg_data) {
	send_time = get_ticks();
	tries_left = RETRIES;
}

void	AckManager::SentPacket::add_recipient(const IPAddress& addr, const PacketHeader& header) {
	recipients.insert(make_pair(addr, header));
}

void AckManager::SentPacket::ack(const IPAddress& peer_addr) {
	recipients.erase(peer_addr);
}

void AckManager::SentPacket::reset_send_time() {
	send_time = get_ticks();
}

uint64_t AckManager::SentPacket::time_since_send() const {
	return get_ticks() - send_time;
}

uint64_t AckManager::SentPacket::time_until_resend() const {
	uint64_t	time_elapsed = time_since_send();
	if (time_elapsed < ACK_TIME) {
		return ACK_TIME - time_elapsed;
	} else {
		return 0;
	}
}

void	AckManager::add_packet(const IPAddress& peer_addr, const PacketHeader& packet_header, const std::string& packet_data) {
	m_packets.push_back(SentPacket(peer_addr, packet_header, packet_data));
	m_packets_by_id.insert(make_pair(make_pair(peer_addr, packet_header.sequence_no), --m_packets.end()));
}

void	AckManager::add_packet(const IPAddress& peer_addr, const Packet& packet) {
	add_packet(peer_addr, packet.header, packet.raw.get_data());
}

AckManager::PacketHandle AckManager::add_broadcast_packet(const std::string& packet_data) {
	m_packets.push_back(SentPacket(packet_data));
	return --m_packets.end();
}

void AckManager::add_broadcast_recipient(AckManager::PacketHandle packet, const IPAddress& peer_addr, const PacketHeader& header) {
	packet->add_recipient(peer_addr, header);
	m_packets_by_id.insert(make_pair(make_pair(peer_addr, header.sequence_no), packet));
}

void AckManager::ack(const IPAddress& peer_addr, uint64_t sequence_no) {
	Map::iterator it(m_packets_by_id.find(make_pair(peer_addr, sequence_no)));
	if (it != m_packets_by_id.end()) {
		it->second->ack(peer_addr);
		if (!it->second->has_recipients()) {
			// no more recipients on this packet
			// that means everyone has received it OK
			// so it can be erased...
			m_packets.erase(it->second);
		}
		m_packets_by_id.erase(it);
	}
}

uint64_t AckManager::time_until_resend() const {
	return m_packets.empty() ? numeric_limits<uint64_t>::max() : m_packets.front().time_until_resend();
}

void AckManager::resend(CommonNetwork& network) {
	while (!m_packets.empty() && m_packets.front().time_until_resend() == 0) {
		SentPacket&	packet(m_packets.front());
		if (packet.tries_left == 0) {
			// Kick every recipient in this packet
			for (map<IPAddress, PacketHeader>::iterator recipient(packet.recipients.begin()); recipient != packet.recipients.end(); ++recipient) {
				network.excessive_packet_drop(recipient->first);
				m_packets_by_id.erase(make_pair(recipient->first, recipient->second.sequence_no));
			}

			m_packets.pop_front();
		} else {
			// Re-send packet to every recipient
			for (map<IPAddress, PacketHeader>::iterator recipient(packet.recipients.begin()); recipient != packet.recipients.end(); ++recipient) {
				network.send_packet(recipient->first, recipient->second, packet.packet_data);
			}

			// Mark that this packet has been re-sent
			--packet.tries_left;
			packet.reset_send_time();

			// Move to the back of the queue
			m_packets.splice(m_packets.end(), m_packets, m_packets.begin());
		}
	}
}

void	AckManager::clear() {
	m_packets.clear();
	m_packets_by_id.clear();
}

void	AckManager::clear_peer(const IPAddress& peer) {
	pair<IPAddress, uint64_t>	search_key(peer, 0);
	Map::iterator			it(m_packets_by_id.lower_bound(search_key));

	while (it != m_packets_by_id.end() && it->first.first == peer) {
		it->second->ack(peer); // simulate an ACK on this packet
		if (!it->second->has_recipients()) {
			// no more recipients on this packet, so erase it
			m_packets.erase(it->second);
		}
		m_packets_by_id.erase(it++);
	}
}

