/*
 * common/PacketQueue.cpp
 *
 * This file is part of Leges Motus, a networked, 2D shooter set in zero gravity.
 * 
 * Copyright 2009-2011 Andrew Ayer, Nathan Partlan, Jeffrey Pfau
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

#include "PacketQueue.hpp"

using namespace LM;
using namespace std;


PacketQueue::PacketQueue(uint64_t next_expected_sequence_no, size_t max_size) {
	m_next_expected_sequence_no = next_expected_sequence_no;
	m_max_size = max_size;
}

bool PacketQueue::push_r(const PacketReader& packet) {
	if (packet.sequence_no() < m_next_expected_sequence_no) {
		// This packet is a dupe of one that already arrived
		return false;
	} else if (packet.sequence_no() == m_next_expected_sequence_no) {
		// This packet arrived in its expected order -- It's ready to be processed NOW
		++m_next_expected_sequence_no;
		return true;
	}

	// This packet arrived out of order -- Add it to the queue!
	list<PacketReader>::reverse_iterator	it(m_queued_packets_r.rbegin());

	while (it != m_queued_packets_r.rend() && it->sequence_no() > packet.sequence_no()) {
		++it;
	}

	// Make sure we don't add a packet that's already in the queue
	if (it != m_queued_packets_r.rend() && it->sequence_no() == packet.sequence_no()) {
		return false;
	}

	// Make sure there's room in the queue for another packet
	if (m_queued_packets_r.size() >= m_max_size) {
		throw FullQueueException();
	}

	m_queued_packets_r.insert(it.base(), packet);
	return false;
}

bool PacketQueue::push(const Packet& packet) {
	if (packet.header.sequence_no < m_next_expected_sequence_no) {
		// This packet is a dupe of one that already arrived
		return false;
	} else if (packet.header.sequence_no == m_next_expected_sequence_no) {
		// This packet arrived in its expected order -- It's ready to be processed NOW
		++m_next_expected_sequence_no;
		return true;
	}

	// This packet arrived out of order -- Add it to the queue!
	list<Packet>::reverse_iterator it(m_queued_packets.rbegin());

	while (it != m_queued_packets.rend() && it->header.sequence_no > packet.header.sequence_no) {
		++it;
	}

	// Make sure we don't add a packet that's already in the queue
	if (it != m_queued_packets.rend() && it->header.sequence_no == packet.header.sequence_no) {
		return false;
	}

	// Make sure there's room in the queue for another packet
	if (m_queued_packets.size() >= m_max_size) {
		throw FullQueueException();
	}

	m_queued_packets.insert(it.base(), packet);
	return false;
}

PacketReader& PacketQueue::peek_r() {
	if (!has_packet()) {
		throw EmptyQueueException();
	}
	return m_queued_packets_r.front();
}

Packet& PacketQueue::peek() {
	if (!has_packet()) {
		throw EmptyQueueException();
	}
	return m_queued_packets.front();
}

void	PacketQueue::pop_r() {
	if (!has_packet()) {
		throw EmptyQueueException();
	}
	m_queued_packets_r.pop_front();
	++m_next_expected_sequence_no;
}

void PacketQueue::pop() {
	if (!has_packet()) {
		throw EmptyQueueException();
	}
	m_queued_packets.pop_front();
	++m_next_expected_sequence_no;
}

void PacketQueue::init(uint64_t next_expected_sequence_no, size_t max_size) {
	m_next_expected_sequence_no = next_expected_sequence_no;
	m_max_size = max_size;
	m_queued_packets.clear();
}

