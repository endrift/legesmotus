/*
 * common/AckManager.cpp
 *
 * Copyright 2009 - Nathan Partlan, Andrew Ayer, Daniel Schneider, and Jeffrey Pfau
 * 
 */

#include "AckManager.hpp"
#include "PacketWriter.hpp"
#include "PacketWriter.hpp"
#include "timer.hpp"
#include <limits>

using namespace std;

AckManager::SentPacket::SentPacket(uint32_t arg_peer_id, const PacketWriter& arg_packet) : data(arg_packet.packet_data()) {
	send_time = get_ticks();
	peer_ids.insert(arg_peer_id);
	packet_type = arg_packet.packet_type();
	packet_id = arg_packet.packet_id();
	tries_left = RETRIES;
}

AckManager::SentPacket::SentPacket(const set<uint32_t>& arg_peer_ids, const PacketWriter& arg_packet) : peer_ids(arg_peer_ids), data(arg_packet.packet_data()) {
	send_time = get_ticks();
	packet_type = arg_packet.packet_type();
	packet_id = arg_packet.packet_id();
	tries_left = RETRIES;
}

void AckManager::SentPacket::ack(uint32_t peer_id) {
	peer_ids.erase(peer_id);
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

void AckManager::add_packet(uint32_t peer_id, const PacketWriter& packet) {
	m_packets.push_back(SentPacket(peer_id, packet));
	m_packets_by_id.insert(make_pair(packet.packet_id(), --m_packets.end()));
}

void AckManager::add_broadcast_packet(const set<uint32_t>& peer_ids, const PacketWriter& packet) {
	m_packets.push_back(SentPacket(peer_ids, packet));
	m_packets_by_id.insert(make_pair(packet.packet_id(), --m_packets.end()));
}

void AckManager::ack(uint32_t peer_id, uint32_t packet_id) {
	std::map<uint32_t, Queue::iterator>::iterator it(m_packets_by_id.find(packet_id));
	if (it != m_packets_by_id.end()) {
		it->second->ack(peer_id);
		if (!it->second->has_peers()) {
			// no more peers on this packet -> erase it
			m_packets.erase(it->second);
			m_packets_by_id.erase(it);
		}
	}
}

uint64_t AckManager::time_until_resend() const {
	return m_packets.empty() ? numeric_limits<uint64_t>::max() : m_packets.front().time_until_resend();
}

void AckManager::resend() {
	while (!m_packets.empty() && m_packets.front().time_until_resend() == 0) {
		SentPacket&	packet(m_packets.front());
		if (packet.tries_left == 0) {
			// Kick every peer in this packet
			for (set<uint32_t>::iterator peer_id(packet.peer_ids.begin()); peer_id != packet.peer_ids.end(); ++peer_id) {
				kick_peer(*peer_id);
			}

			// Remove from containers
			m_packets_by_id.erase(packet.packet_id);
			m_packets.pop_front();
		} else {
			// Re-send packet to every peer
			for (set<uint32_t>::iterator peer_id(packet.peer_ids.begin()); peer_id != packet.peer_ids.end(); ++peer_id) {
				resend_packet(*peer_id, packet.data);
			}

			// Mark that this packet has been re-sent
			--packet.tries_left;
			packet.reset_send_time();

			// Move to the back of the queue
			m_packets.splice(m_packets.end(), m_packets, m_packets.begin());
		}
	}
}

