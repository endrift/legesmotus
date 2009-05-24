/*
 * common/IPAddress.cpp
 *
 * Copyright 2009 - Nathan Partlan, Andrew Ayer, Daniel Schneider, and Jeffrey Pfau
 * 
 */

#include "IPAddress.hpp"

#ifndef __WIN32
#include <sys/socket.h>
#endif

IPAddress::IPAddress() {
	host = 0;
	port = 0;
}

IPAddress::IPAddress(uint32_t arg_host, uint16_t arg_port) {
	host = arg_host;
	port = arg_port;
}

IPAddress::IPAddress(const struct sockaddr_in& addr) {
	host = addr.sin_addr.s_addr;
	port = addr.sin_port;
}

void IPAddress::populate_sockaddr(struct sockaddr_in& addr) const {
	addr.sin_addr.s_addr = host;
	addr.sin_port = port;
	addr.sin_family = AF_INET;
}

void IPAddress::set_host(const struct in_addr& addr) {
	host = addr.s_addr;
}

bool IPAddress::is_localhost() const {
	return ntohl(host) >> 24 == 127;
}
