/*
 * common/RawPacket.hpp
 *
 * Copyright 2009 - Nathan Partlan, Andrew Ayer, Daniel Schneider, and Jeffrey Pfau
 * 
 */

#ifndef LM_COMMON_RAWPACKET_HPP
#define LM_COMMON_RAWPACKET_HPP

#include "SDL_net.h"
#include "LMException.hpp"
#include <string>

class PacketWriter;

/*
 * A raw packet represents a raw SDL UDP packet.
 * It handles automatic allocation and de-allocation and provides convenient functions for filling it.
 * It's also fully compatible with SDL's UDPpacket, so it can be passed in lieu of one to SDL_net functions.
 */
class RawPacket {
private:
	size_t			m_max_size;		// Maximum size of the packet
	UDPpacket*		m_packet;		// The underlying packet

	// Forbid copy and assignment of this class, because it's not safe
	RawPacket (const RawPacket&) { }
	RawPacket& operator= (const RawPacket&) { return *this; }
public:
	// Construct a raw packet with given maximum size
	explicit RawPacket(size_t max_size) throw(LMException);
	~RawPacket();

	// Set the address of this packet:
	void			set_address(const IPaddress& address);
	// Fill this packet with the given data of the given length:
	void			fill(const char* data, size_t length);
	// Fill this packet with the data in the given PacketWriter:
	void			fill(const PacketWriter& data);

	// Allows you to access fields of the underlying SDL UDPPacket by using the -> operator:
	UDPpacket* operator-> () { return m_packet; }
	const UDPpacket* operator-> () const { return m_packet; }

	// Allows this RawPacket to be implicitly cast to a UDPpacket*
	operator UDPpacket* () { return m_packet; }
	operator const UDPpacket* () const { return m_packet; }
};

#endif
