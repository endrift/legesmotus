/*
 * common/PacketReader.hpp
 *
 * Copyright 2009 - Nathan Partlan, Andrew Ayer, Daniel Schneider, and Jeffrey Pfau
 * 
 */

#ifndef LM_COMMON_PACKETREADER_HPP
#define LM_COMMON_PACKETREADER_HPP

#include "network.hpp"
#include "StringTokenizer.hpp"
#include <string>
#include <stdint.h>
#include <iosfwd>

class UDPPacket;

/*
 * The packet reader provides a convenient inteface for reading fields from packets.
 * You can access the packet type and packet ID at any time,
 * and read fields sequentially from the packet using the >> operator.
 *
 * Example:
 * 	PacketReader	reader("4\f15123512\f5\fB\fCover me, I'm going for the gate!");
 * 	reader.packet_type();	// Would return 4
 * 	reader.packet_id();	// Would return 15123512
 *
 * 	int		sender_id;
 * 	std::string	recipient_id;
 * 	std::string	message_text;
 * 	reader >> sender_id >> recipient_id >> message_text;
 *
 *	// sender_id is now 5
 *	// recipient_id is now "B"
 *	// message_text is now "Cover me, I'm going for the gate!"
 */

class PacketReader : private StringTokenizer {
private:
	uint32_t	m_packet_type;
	uint32_t	m_packet_id;

public:
	// Construct a packet reader from the given raw packet data
	explicit PacketReader(const char* packet_data, char separator =PACKET_FIELD_SEPARATOR);
	explicit PacketReader(const UDPPacket& packet);

	// Get the packet type and packet ID at any time:
	uint32_t	packet_type() const { return m_packet_type; }
	uint32_t	packet_id() const { return m_packet_id; }

	// Import operations from the base class:
	using StringTokenizer::discard_next;
	using StringTokenizer::get_rest;
	using StringTokenizer::operator>>;
	using StringTokenizer::has_more;

	// To test whether there are any tokens left for processing
	bool		operator! () const { return !has_more(); }
			operator const void* () const { return has_more() ? this : NULL; }
};

// Write the remaining un-processed raw packet data to an output stream:
std::ostream&	operator<<(std::ostream& out, const PacketReader& packet_reader);

#endif
