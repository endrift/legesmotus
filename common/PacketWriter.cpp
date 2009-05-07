/*
 * common/PacketWriter.cpp
 *
 * Copyright 2009 - Nathan Partlan, Andrew Ayer, Daniel Schneider, and Jeffrey Pfau
 * 
 */

#include "PacketWriter.hpp"
#include <iomanip>

// See .hpp file for extensive comments.

using namespace std;

uint32_t	PacketWriter::m_next_packet_id = 1;

PacketWriter::PacketWriter(uint32_t packet_type) {
	m_out.setf(ios::boolalpha);		// use true/false when outputting bools
	m_packet_type = packet_type;
	m_packet_id = m_next_packet_id++;

	// Write the first 2 fields, which are always packet type and packet ID.
	m_out << m_packet_type << PACKET_FIELD_SEPARATOR << m_packet_id;
}

