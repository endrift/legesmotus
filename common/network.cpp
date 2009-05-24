/*
 * common/network.cpp
 *
 * Copyright 2009 - Nathan Partlan, Andrew Ayer, Daniel Schneider, and Jeffrey Pfau
 * 
 */

#include "network.hpp"
#ifdef __WIN32
#include <Winsock2.h>
#else
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#endif
#include <stdio.h>
#include <sstream>

using namespace std;

string	format_ip_address(const IPAddress& addr, bool resolve) {
	string			hostname;

	if (resolve && resolve_ip_address(hostname, NULL, addr)) {
		ostringstream	full_name;
		full_name << hostname << ':' << ntohs(addr.port);
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

bool	resolve_hostname(IPAddress& resolved_addr, const char* hostname_to_resolve, uint16_t portno) {
	resolved_addr.port = htons(portno);

	struct in_addr		addr;
	if (inet_aton(hostname_to_resolve, &addr)) {
		// IP address specified for hostname - no need to resolve
		resolved_addr.set_host(addr);
		return true;
	} else {
		// Attempt to resolve
		struct hostent*	host = gethostbyname(hostname_to_resolve);
		if (host && host->h_addrtype == AF_INET && host->h_length == 4) {
			memcpy(&resolved_addr.host, host->h_addr_list[0], 4);
			return true;
		}
	}
	
	return false;
}

bool	resolve_ip_address(std::string& resolved_hostname, uint16_t* portno, const IPAddress& address_to_resolve) {
	if (portno) {
		*portno = ntohs(address_to_resolve.port);
	}

	// Attempt to resolve
	struct hostent*	host = gethostbyaddr(reinterpret_cast<const char*>(&address_to_resolve.host), 4, AF_INET);
	if (host && host->h_name) {
		resolved_hostname = host->h_name;
		return true;
	}

	return false;
}

#ifdef __WIN32

int	inet_aton(const char* cp, struct in_addr* inp) {
	if (cp) {
		unsigned long addr = inet_addr(cp);
		if (addr != INADDR_NONE) {
			// Valid address
			inp->s_addr = addr;
			return 1;
		}
	}
	return 0;
}

#endif

