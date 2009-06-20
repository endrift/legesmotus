/*
 * common/IPAddress.cpp
 *
 * This file is part of Leges Motus, a networked, 2D shooter set in zero gravity.
 * 
 * Copyright 2009 Andrew Ayer, Nathan Partlan, Jeffrey Pfau
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

#include "IPAddress.hpp"
#include "network.hpp"
#include "StringTokenizer.hpp"
#include <ostream>
#include <string>

#ifndef __WIN32
#include <sys/socket.h>
#endif

using namespace LM;

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


std::ostream&		LM::operator<< (std::ostream& out, const IPAddress& addr) {
	return out << format_ip_address(addr);
}

StringTokenizer&	LM::operator>> (StringTokenizer& tok, IPAddress& addr) {
	if (const char* str = tok.get_next()) {
		std::string	host_part;
		uint16_t	port_part;
		StringTokenizer(str, ':') >> host_part >> port_part;

		if (!resolve_hostname(addr, host_part.c_str(), port_part)) {
			addr.clear();
		}
	} else {
		addr.clear();
	}
	return tok;
}

