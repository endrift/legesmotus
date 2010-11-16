/*
 * common/PacketReader.hpp
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

#ifndef LM_COMMON_PACKETREADER_HPP
#define LM_COMMON_PACKETREADER_HPP

#include "network.hpp"
#include "PacketHeader.hpp"
#include "StringTokenizer.hpp"
#include <string>
#include <stdint.h>
#include <iosfwd>
#include <algorithm>

namespace LM {
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
	
	class PacketReader : public StringTokenizer {
	private:
		PacketHeader	m_header;
	
	public:
		// Construct a packet reader from the given raw packet data
		explicit PacketReader(const char* packet_data, char separator =PACKET_FIELD_SEPARATOR);
		explicit PacketReader(const UDPPacket& packet);
	
		const PacketHeader& get_header() const { return m_header; }
		// Get the packet type and sequence NO at any time:
		uint32_t	packet_type() const { return m_header.packet_type; }
		uint64_t	sequence_no() const { return m_header.sequence_no; }
		uint32_t	connection_id() const { return m_header.connection_id; }
	
		// Import operations from the base class:
		using StringTokenizer::discard_next;
		using StringTokenizer::get_rest;
		using StringTokenizer::operator>>;
		using StringTokenizer::has_more;
	
		// To test whether there are any tokens left for processing
		bool		operator! () const { return !has_more(); }
				operator const void* () const { return has_more() ? this : NULL; }

		void		swap(PacketReader& other);
	};
	
	// Write the remaining un-processed raw packet data to an output stream:
	std::ostream&	operator<<(std::ostream& out, const PacketReader& packet_reader);
}

namespace std {
	template<> inline void swap (LM::PacketReader& x, LM::PacketReader& y) { x.swap(y); }
}

#endif
