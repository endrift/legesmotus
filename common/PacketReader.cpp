/*
 * common/PacketReader.cpp
 *
 * Copyright 2009 - Nathan Partlan, Andrew Ayer, Daniel Schneider, and Jeffrey Pfau
 * 
 */

#include "PacketReader.hpp"
#include "Point.hpp"
#include <cstring>
#include <cstdlib>
#include <ostream>

using namespace std;

const char*	PacketReader::get_next() {
	if (m_next_field == NULL) {
		// Already at end
		return NULL;
	}

	// Remember where we started.
	char*		start = m_next_field;

	// Advance until we either hit the end of the data or reach a field separator
	while (*m_next_field != '\0' && *m_next_field != m_separator) {
		++m_next_field;
	}
	
	if (*m_next_field == '\0') {
		m_next_field = NULL; // End of packet data -- invalidate the reader
	} else {
		*m_next_field++ = '\0'; // Overwrite the field separator
	}
	
	return start;
}



PacketReader::PacketReader(const char* packet_data, char separator) {
	m_separator = separator;

	// Copy in the packet data
	m_buffer = new char[strlen(packet_data) + 1];
	strcpy(m_buffer, packet_data);
	// Start at the beginning
	m_next_field = m_buffer;

	// Process the first two fields, which are always packet type and packet ID
	*this >> m_packet_type >> m_packet_id;
}

PacketReader::PacketReader(const RawPacket& packet) {
	m_separator = PACKET_FIELD_SEPARATOR;

	// Copy in the packet data
	m_buffer = new char[packet->len + 1];
	memcpy(m_buffer, packet->data, packet->len);
	m_buffer[packet->len] = '\0';	// Assume the raw packet data does NOT include a null terminator
	// Start at the beginning
	m_next_field = m_buffer;

	// Process the first two fields, which are always packet type and packet ID
	*this >> m_packet_type >> m_packet_id;
}

PacketReader::~PacketReader() {
	delete[] m_buffer;
}

const char*	PacketReader::get_rest() const {
	return m_next_field;
}

PacketReader&	PacketReader::operator>> (bool& b)
{
	// True if:
	//  - "yes"
	//  - "true"
	//  - "on"
	//  - positive integer

	const char*	p = get_next();

	b = p && (strcasecmp(p, "yes") == 0 || strcasecmp(p, "true") == 0 || strcasecmp(p, "on") == 0 || atoi(p) > 0);

	return *this;
}


PacketReader&	PacketReader::operator>> (char& c)
{
	const char*	p = get_next();
	c = p ? *p : 0;
	return *this;
}


PacketReader&	PacketReader::operator>> (short& i)
{
	const char*	p = get_next();
	i = p ? short(atoi(p)) : 0;
	return *this;
}


PacketReader&	PacketReader::operator>> (unsigned short& i)
{
	const char*	p = get_next();
	i = p ? (unsigned short)(atoi(p)) : 0;
	return *this;
}


PacketReader&	PacketReader::operator>> (int& i)
{
	const char*	p = get_next();
	i = p ? atoi(p) : 0;
	return *this;
}


PacketReader&	PacketReader::operator>> (unsigned int& i)
{
	const char*	p = get_next();
	i = p ? (unsigned int)(atoi(p)) : 0;
	return *this;
}


PacketReader&	PacketReader::operator>> (long& i)
{
	const char*	p = get_next();
	i = p ? atol(p) : 0;
	return *this;
}


PacketReader&	PacketReader::operator>> (unsigned long& i)
{
	const char*	p = get_next();
	i = p ? strtoul(p, 0, 10) : 0;
	return *this;
}


PacketReader&	PacketReader::operator>> (float& i)
{
	const char*	p = get_next();
	i = p ? strtof(p, 0) : 0.0;
	return *this;
}


PacketReader&	PacketReader::operator>> (double& i)
{
	const char*	p = get_next();
	i = p ? atof(p) : 0;
	return *this;
}


PacketReader&	PacketReader::operator>> (std::string& s)
{
	if (const char* p = get_next())
		s = p;
	else
		s.clear();

	return *this;
}

PacketReader&	PacketReader::operator>> (Point& point) {
	if (const char* p = get_next()) {
		point.init_from_string(p);
	} else {
		point.clear();
	}
	return *this;
}


std::ostream&	operator<<(std::ostream& out, const PacketReader& packet_reader) {
	return out << packet_reader.get_rest();
}

