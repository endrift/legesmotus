/*
 * common/RawPacket.cpp
 *
 * Copyright 2009 - Nathan Partlan, Andrew Ayer, Daniel Schneider, and Jeffrey Pfau
 * 
 */

#include "RawPacket.hpp"
#include "PacketWriter.hpp"
#include "LMException.hpp"
#include <sstream>
#include <cstring>

// See .hpp file for extensive comments.

using namespace std;

RawPacket::RawPacket(size_t max_size) throw(LMException) {
	// Allocate the SDL packet here.
	m_max_size = max_size;
	m_packet = SDLNet_AllocPacket(max_size);
	if (!m_packet) {
		// Should never happen
		ostringstream	msg;
		msg << "Failed to allocate UDP packet of size " << max_size << ": " << SDLNet_GetError();
		throw LMException(msg.str());
	}
}

RawPacket::~RawPacket() {
	SDLNet_FreePacket(m_packet);
}

void	RawPacket::set_address(const IPaddress& address) {
	m_packet->address.host = address.host;
	m_packet->address.port = address.port;
}

void	RawPacket::fill(const char* data, size_t data_length) {
	// Don't store the null terminator in the raw data.
	m_packet->len = std::min(m_max_size, data_length);
	memcpy(m_packet->data, data, m_packet->len);
}

void	RawPacket::fill(const PacketWriter& packet) {
	std::string	data(packet.packet_data());
	fill(data.c_str(), data.size());
}
