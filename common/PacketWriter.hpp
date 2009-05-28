/*
 * common/PacketWriter.hpp
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

#ifndef LM_COMMON_PACKETWRITER_HPP
#define LM_COMMON_PACKETWRITER_HPP

#include <sstream>
#include <string>
#include <stdint.h>
#include <stddef.h>
#include "network.hpp"

/*
 * The packet reader provides a convenient inteface for sequentially writing fields to packets.
 * You construct it with a packet type, and then use the << operator to write fields of any type.
 * 
 * Example:
 * 	PacketWriter		packet(MESSAGE_PACKET);
 * 	packet << this_player_id << recipient_player_id << message_text;
 *
 * 	packet.packet_data(); // Would return something like "4\f41251231\f5\fB\fCover me, I'm going for the gate!"
 */

class PacketWriter {
private:
	// Raw packet data is written into this string stream:
	std::ostringstream	m_out;

	// The next packet ID to use - starts at 1 and increments:
	static uint32_t		m_next_packet_id;

	// The packet type and ID for this packet:
	uint32_t		m_packet_type;
	uint32_t		m_packet_id;

public:
	explicit PacketWriter(uint32_t packet_type);

	// Getters
	uint32_t	packet_type() const { return m_packet_type; }
	uint32_t	packet_id() const { return m_packet_id; }

	// Get the raw packet data:
	std::string	packet_data() const { return m_out.str(); }

	// Write a field into the packet:
	template<class T> PacketWriter& operator<<(const T& obj) {
		m_out << PACKET_FIELD_SEPARATOR << obj;
		return *this;
	}

};

#endif
