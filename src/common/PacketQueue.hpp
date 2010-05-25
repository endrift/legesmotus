/*
 * common/PacketQueue.hpp
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

#ifndef LM_COMMON_PACKETQUEUE_HPP
#define LM_COMMON_PACKETQUEUE_HPP

#include <list>
#include "PacketReader.hpp"

namespace LM {
	class PacketQueue {
	private:
		size_t			m_max_size;
		uint64_t		m_next_expected_sequence_no;
		std::list<PacketReader>	m_queued_packets;

	public:
		explicit PacketQueue(uint64_t next_expected_sequence_no =1, size_t max_size =256);

		void			init(uint64_t next_expected_sequence_no =1, size_t max_size =256);
		void			set_max_size(size_t max_size) { m_max_size = max_size; }

		// Returns true if the given packet is ready to be processed NOW:
		bool			push(const PacketReader& packet);

		// Returns true if there is a packet ready to be processed:
		bool			has_packet() const { return !m_queued_packets.empty() && m_queued_packets.front().sequence_no() == m_next_expected_sequence_no; }

		// Return the next packet that is ready to be processed (ONLY CALL if has_packet() returns TRUE!)
		PacketReader&		peek();

		// Pop the next packet that was ready to be processed (ONLY CALL if has_packet() returns TRUE!)
		void			pop();

		class EmptyQueueException { };
		class FullQueueException { };
	};
}

#endif
