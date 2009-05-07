/*
 * common/PacketReader.hpp
 *
 * Copyright 2009 - Nathan Partlan, Andrew Ayer, Daniel Schneider, and Jeffrey Pfau
 * 
 */

#ifndef LM_COMMON_PACKETREADER_HPP
#define LM_COMMON_PACKETREADER_HPP

#include "RawPacket.hpp"
#include "SDL_net.h"
#include "network.hpp"
#include <string>
#include <stdint.h>
#include <iosfwd>

class Point;

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

class PacketReader {
private:
	uint32_t	m_packet_type;
	uint32_t	m_packet_id;

	char		m_separator;	// The character that separates fields in the packet
	char*		m_buffer;	// The packet data is stored in here.
	char*		m_next_field;	// Points to the field that will be processed next. (NULL if at end)

	const char*	get_next ();	// Advance to the next field and return the old one.

public:
	// Construct a packet reader from the given raw packet data
	explicit PacketReader(const char* packet_data, char separator =PACKET_FIELD_SEPARATOR);
	explicit PacketReader(const RawPacket& packet);
	~PacketReader();

	// Get the packet type and packet ID at any time:
	uint32_t	packet_type() const { return m_packet_type; }
	uint32_t	packet_id() const { return m_packet_id; }

	// Discard (i.e. ignore) the next field
	void		discard_next() { get_next(); }

	// Get the remaining un-processed packet data in raw form:
	const char*	get_rest () const;

	// The following functions read the next field into the variable of the given type:
	PacketReader&	operator>> (bool&);
	PacketReader&	operator>> (char&);
	PacketReader&	operator>> (unsigned char&);

	PacketReader&	operator>> (short&);
	PacketReader&	operator>> (unsigned short&);

	PacketReader&	operator>> (int&);
	PacketReader&	operator>> (unsigned int&);

	PacketReader&	operator>> (long&);
	PacketReader&	operator>> (unsigned long&);

	PacketReader&	operator>> (float&);
	PacketReader&	operator>> (double&);

	PacketReader&	operator>> (Point&);

	PacketReader&	operator>> (std::string&);

	// Is there another field to be read?
	bool		has_more() const { return m_next_field != NULL; }

	// To test whether there are any fields left for processing
	bool		operator! () const { return !has_more(); }
			operator const void* () const { return has_more() ? this : NULL; }

};

// Write the remaining un-processed raw packet data to an output stream:
std::ostream&	operator<<(std::ostream& out, const PacketReader& packet_reader);

#endif
