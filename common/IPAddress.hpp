/*
 * common/IPAddress.hpp
 *
 * Copyright 2009 - Nathan Partlan, Andrew Ayer, Daniel Schneider, and Jeffrey Pfau
 * 
 */

#ifndef LM_COMMON_IPADDRESS_HPP
#define LM_COMMON_IPADDRESS_HPP

#ifdef __WIN32
#include "Winsock2.h"
#else
#include <netinet/in.h>
#endif

#include <stdint.h>

class IPAddress {
public:
	uint32_t	host;
	uint16_t	port;

	IPAddress();
	IPAddress(uint32_t host, uint16_t port);
	IPAddress(const struct sockaddr_in& address);

	void		populate_sockaddr(struct sockaddr_in& address) const;
	void		set_host(const struct in_addr& addr);

	bool		is_localhost() const;

	bool		operator==(const IPAddress& other) const {
		return host == other.host && port == other.port;
	}
	bool		operator<(const IPAddress& other) const {
		return host == other.host ? port < other.port : host < other.host;
	}
};

#endif
