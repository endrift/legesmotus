/*
 * common/PacketReader.cpp
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

#include "PacketReader.hpp"
#include "UDPPacket.hpp"
#include <cstring>
#include <cstdlib>
#include <ostream>
#include <algorithm>

// See .hpp file for extensive comments.

using namespace LM;
using namespace std;

PacketReader::PacketReader(const char* packet_data, char separator) : StringTokenizer(packet_data, separator) {
	// Process the packet header, which consists of the first fields
	m_header.read(*this);
}

PacketReader::PacketReader(const UDPPacket& packet) {
	StringTokenizer::set_delimiter(PACKET_FIELD_SEPARATOR);
	StringTokenizer::init_from_raw_data(packet.get_data(), packet.get_length(), false);
	// Process the packet header, which consists of the first fields
	m_header.read(*this);
}

std::ostream&   LM::operator<<(std::ostream& out, const PacketReader& packet_reader) {
	return out << packet_reader.get_rest();
}

void	PacketReader::swap(PacketReader& other) {
	StringTokenizer::swap(other);
	// std:: prefix necessary here to avoid name conflicts
	std::swap(m_header, other.m_header);
}

