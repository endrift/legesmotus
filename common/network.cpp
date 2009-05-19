/*
 * common/network.cpp
 *
 * Copyright 2009 - Nathan Partlan, Andrew Ayer, Daniel Schneider, and Jeffrey Pfau
 * 
 */

#include "network.hpp"
#include "compat_sdl.h"
#include "SDL_net.h"
#ifdef __WIN32
#include <Winsock2.h>
#else
#include <arpa/inet.h>
#endif
#include <stdio.h>
#include <sstream>

using namespace std;

string	format_ip_address(const IPaddress& addr, bool resolve) { // XXX: this function probably doesn't work in Windows (use of ntohl and ntohs)
	const char*		name = NULL;
	if (resolve && (name = SDLNet_ResolveIP(const_cast<IPaddress*>(&addr))) != NULL) {
		ostringstream	full_name;
		full_name << name << ':' << ntohs(addr.port);
		return full_name.str();
	}

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
