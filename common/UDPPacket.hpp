/*
 * common/UDPPacket.hpp
 *
 * Copyright 2009 - Nathan Partlan, Andrew Ayer, Daniel Schneider, and Jeffrey Pfau
 * 
 */

#ifndef LM_COMMON_UDPPACKET_HPP
#define LM_COMMON_UDPPACKET_HPP

#include "common/IPAddress.hpp"
#include <string>

class PacketWriter;

class UDPPacket {
private:
	char*		m_data;
	size_t		m_length;
	size_t		m_max_length;
	IPAddress	m_address;

	friend class UDPSocket;
public:
	explicit UDPPacket(size_t max_length);
	UDPPacket(const UDPPacket& other);
	~UDPPacket();

	UDPPacket&		operator=(const UDPPacket& other);

	const char*		get_data() const { return m_data; }
	size_t			get_length() const { return m_length; }
	size_t			get_max_length() const { return m_max_length; }

	const IPAddress&	get_address() const { return m_address; }
	void			set_address(const IPAddress& address);

	// Fill this packet with the given data of the given length:
	void			fill(const char* data, size_t length);
	// Fill this packet with the data in the given PacketWriter:
	void			fill(const PacketWriter& data);
	// Fill this packet with the data in the given string:
	void			fill(const std::string& data);

	void			clear();
};

#endif
