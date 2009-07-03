/*
 * common/IPAddress.hpp
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

#ifndef LM_COMMON_IPADDRESS_HPP
#define LM_COMMON_IPADDRESS_HPP

#ifdef __WIN32
#include "Winsock2.h"
#else
#include <netinet/in.h>
#endif

#include <stdint.h>
#include <iosfwd>

namespace LM {
	class StringTokenizer;
	
	class IPAddress {
	public:
		uint32_t	host;
		uint16_t	port;
	
		IPAddress();
		IPAddress(uint32_t host, uint16_t port);
		IPAddress(const struct sockaddr_in& address);
	
		void		clear () { host = 0; port = 0; }
	
		void		populate_sockaddr(struct sockaddr_in& address) const;
		void		set_host(const struct in_addr& addr);
	
		bool		is_localhost() const;
	
		bool		operator!=(const IPAddress& other) const {
			return host != other.host || port != other.port;
		}
		bool		operator==(const IPAddress& other) const {
			return host == other.host && port == other.port;
		}
		bool		operator<(const IPAddress& other) const {
			return host == other.host ? port < other.port : host < other.host;
		}
	};
	
	std::ostream&		operator<< (std::ostream&, const IPAddress&);
	StringTokenizer&	operator>> (StringTokenizer&, IPAddress&);
	
}

#endif
