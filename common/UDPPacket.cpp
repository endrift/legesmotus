/*
 * common/UDPPacket.cpp
 *
 * Copyright 2009 - Nathan Partlan, Andrew Ayer, Daniel Schneider, and Jeffrey Pfau
 * 
 */

#include "UDPPacket.hpp"
#include <cstring>
#include <string>
#include "PacketWriter.hpp"

using namespace std;

UDPPacket::UDPPacket(size_t max_length) {
	m_max_length = max_length;
	m_length = 0;
	m_data = new char[m_max_length];
}

UDPPacket::UDPPacket(const UDPPacket& other) {
	m_max_length = other.m_max_length;
	m_length = other.m_length;
	m_data = new char[m_max_length];
	m_address = other.m_address;
	memcpy(m_data, other.m_data, m_length);
}

UDPPacket::~UDPPacket() {
	delete[] m_data;
}

UDPPacket&	UDPPacket::operator=(const UDPPacket& other) {
	if (m_max_length != other.m_max_length) {
		char*	new_data = new char[other.m_max_length];
		delete[] m_data;
		m_data = new_data;
		m_max_length = other.m_max_length;
	}

	m_length = other.m_length;
	m_address = other.m_address;
	memmove(m_data, other.m_data, m_length);

	return *this;
}

void	UDPPacket::set_address(const IPAddress& address) {
	m_address = address;
}

void	UDPPacket::clear() {
	m_length = 0;
}

void	UDPPacket::fill(const char* data, size_t data_length) {
	// Don't store the null terminator in the raw data.
	m_length = std::min(m_max_length, data_length);
	memcpy(m_data, data, m_length);
}

void	UDPPacket::fill(const PacketWriter& packet) {
	fill(packet.packet_data());
}

void	UDPPacket::fill(const string& data) {
	fill(data.c_str(), data.size());
}
