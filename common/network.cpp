/*
 * common/network.cpp
 *
 * Copyright 2009 - Nathan Partlan, Andrew Ayer, Daniel Schneider, and Jeffrey Pfau
 * 
 */

#include "network.hpp"
#include "SDL_net.h"
#include "SDL.h"
#include <arpa/inet.h>
#include <stdio.h>

using namespace std;

string	format_ip_address(const IPaddress& addr) {
	char			buffer[32];
	uint32_t		ip_address = ntohl(addr.host);
	uint16_t		port_no = ntohs(addr.port);
	snprintf(buffer, 32, "%u.%u.%u.%u:%d",
                                        (unsigned int)(ip_address >> 24),
                                        (unsigned int)(ip_address >> 16 & 0xff),
                                        (unsigned int)(ip_address >> 8 & 0xff),
                                        (unsigned int)(ip_address & 0xff),
                                        (int)port_no);

	return buffer;
}
