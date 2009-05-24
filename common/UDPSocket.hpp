/*
 * common/UDPSocket.hpp
 *
 * Copyright 2009 - Nathan Partlan, Andrew Ayer, Daniel Schneider, and Jeffrey Pfau
 * 
 */

#ifndef LM_COMMON_UDPSOCKET_HPP
#define LM_COMMON_UDPSOCKET_HPP

#include <stdint.h>

class UDPPacket;

class UDPSocket {
private:
	int	fd;

	void	init();
	void	close();

public:
	UDPSocket();
	explicit UDPSocket(unsigned int portno);
	~UDPSocket();

	bool	bind(unsigned int portno);

	// return true as soon as a packet is ready for reading
	// will wait for up to wait_time milliseconds for a packet, after which it will return false
	bool	has_packets(uint64_t wait_time =0); 

	bool	send(const UDPPacket&);
	bool	recv(UDPPacket&);

	operator const void* () const { return fd >= 0 ? this : 0; }
	bool	operator! () const { return fd < 0; }

};

#endif
