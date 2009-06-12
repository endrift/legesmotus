/*
 * common/UDPSocket.cpp
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

#include "common/LMException.hpp"
#include "common/UDPPacket.hpp"
#include "common/IPAddress.hpp"
#include "common/network.hpp"
#include "UDPSocket.hpp"

#ifdef __WIN32
#include "Winsock2.h"
#else
#include <sys/types.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#endif


void	UDPSocket::init() {
#ifdef __WIN32__
	static struct WinSock {
		WinSock() {
			WSADATA	wsaData;
			WORD	version = MAKEWORD(2, 0);

			if (WSAStartup(version, &wsaData) != 0) {
				throw LMException("Failed to initialize WinSock");
			}

			if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 0 ) {
				WSACleanup();
				throw LMException("Required version of WinSock not supported");
			}
		}
		~WinSock() {
			WSACleanup();
		}
	} winsock;
#endif

	if ((fd = ::socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		throw LMException("Failed to create UDP socket");
	}

	// The cast to char* is necessary to build on Windows, but should be harmless.
	// On Unix, the char* will be implicitly cast to a void*, which would have happened anyways even if there were no explicit cast.
	int		one = 1;
	setsockopt(fd, SOL_SOCKET, SO_BROADCAST, reinterpret_cast<const char*>(&one), sizeof(one));
}

UDPSocket::UDPSocket() {
	init();
}

UDPSocket::UDPSocket(unsigned int portno) {
	init();
	if (!bind(portno)) {
		close();
	}
}

UDPSocket::UDPSocket(const IPAddress& bind_address) {
	init();
	if (!bind(bind_address)) {
		close();
	}
}

UDPSocket::UDPSocket(const char* interface_address, unsigned int portno) {
	init();
	if (!bind(interface_address, portno)) {
		close();
	}
}

UDPSocket::~UDPSocket() {
	close();
}

void	UDPSocket::close() {
	if (fd >= 0) {
#ifdef __WIN32
		::closesocket(fd);
#else
		::close(fd);
#endif

		fd = -1;
	}
}

bool	UDPSocket::bind(const char* interface_address, unsigned int portno) {
	struct sockaddr_in	addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	if (interface_address) {
		if (!inet_aton(interface_address, &addr.sin_addr)) {
			return false;
		}
	} else {
		addr.sin_addr.s_addr = htonl(INADDR_ANY);
	}
	addr.sin_port = htons(portno);

	return ::bind(fd, reinterpret_cast<const sockaddr*>(&addr), sizeof(addr)) == 0;
}


bool	UDPSocket::bind(const IPAddress& bind_address) {
	struct sockaddr_in	addr;
	memset(&addr, 0, sizeof(addr));
	bind_address.populate_sockaddr(addr);

	return ::bind(fd, reinterpret_cast<const sockaddr*>(&addr), sizeof(addr)) == 0;
}


bool	UDPSocket::has_packets(uint32_t wait_time) {
	struct timeval	timeout = { wait_time / 1000, (wait_time % 1000) * 1000 };

	fd_set		read_fds;
	FD_ZERO(&read_fds);
	FD_SET(fd, &read_fds);

	int		retval = select(fd + 1, &read_fds, 0, 0, &timeout);

	return retval > 0;
}

bool	UDPSocket::send(const UDPPacket& packet) {
	struct sockaddr_in	addr;
	packet.get_address().populate_sockaddr(addr);

	ssize_t			bytes_sent = sendto(fd, packet.get_data(), packet.get_length(), 0, reinterpret_cast<const sockaddr*>(&addr), sizeof(addr));
	
	return bytes_sent >= 0 && size_t(bytes_sent) == packet.get_length();
}

bool	UDPSocket::recv(UDPPacket& packet) {
	struct sockaddr_in	addr;
	socklen_t		addr_len = sizeof(addr);
	ssize_t			bytes_received = recvfrom(fd, packet.m_data, packet.get_max_length(), 0, reinterpret_cast<sockaddr*>(&addr), &addr_len);

	if (bytes_received < 0) {
		packet.clear();
		return false;
	}

	packet.set_address(addr);
	packet.m_length = bytes_received;
	return true;
}

