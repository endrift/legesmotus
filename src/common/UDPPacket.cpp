/*
 * common/UDPPacket.cpp
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

#include "UDPPacket.hpp"
#include <cstring>
#include <string>
#include "PacketWriter.hpp"

using namespace LM;
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
	clear();
	append(data, data_length);
}

void	UDPPacket::fill(const string& data) {
	fill(data.c_str(), data.size());
}

void	UDPPacket::append(const char* data, size_t data_length) {
	// Note: don't store the null terminator in the raw data.
	data_length = std::min(m_max_length - m_length, data_length);
	memcpy(m_data + m_length, data, data_length);
	m_length += data_length;
}

void	UDPPacket::append(const string& data) {
	append(data.c_str(), data.size());
}
