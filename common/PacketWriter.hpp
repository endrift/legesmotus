/*
 * common/PacketWriter.hpp
 *
 * Copyright 2009 - Nathan Partlan, Andrew Ayer, Daniel Schneider, and Jeffrey Pfau
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

	uint32_t	packet_type() const { return m_packet_type; }
	uint32_t	packet_id() const { return m_packet_id; }

	// Get the raw packet data:
	std::string	packet_data() const { return m_out.str() + "\n"; }

	// Write a field into the packet:
	template<class T> PacketWriter& operator<<(const T& obj) {
		m_out << PACKET_FIELD_SEPARATOR << obj;
		return *this;
	}

};

#endif
