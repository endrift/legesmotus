/*
 * common/PacketReader.cpp
 *
 * Copyright 2009 - Nathan Partlan, Andrew Ayer, Daniel Schneider, and Jeffrey Pfau
 * 
 */

#include "PacketReader.hpp"
#include "UDPPacket.hpp"
#include "Point.hpp"
#include <cstring>
#include <cstdlib>
#include <ostream>

// See .hpp file for extensive comments.

using namespace std;

PacketReader::PacketReader(const char* packet_data, char separator) : StringTokenizer(packet_data, separator) {
	// Process the first two fields, which are always packet type and packet ID
	*this >> m_packet_type >> m_packet_id;
}

PacketReader::PacketReader(const UDPPacket& packet) {
	StringTokenizer::init_from_raw_data(packet.get_data(), packet.get_length(), PACKET_FIELD_SEPARATOR);
	// Process the first two fields, which are always packet type and packet ID
	*this >> m_packet_type >> m_packet_id;
}

std::ostream&   operator<<(std::ostream& out, const PacketReader& packet_reader) {
	return out << packet_reader.get_rest();
}

