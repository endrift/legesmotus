/*
 * common/UDPSocket.cpp
 *
 * Copyright 2009 - Nathan Partlan, Andrew Ayer, Daniel Schneider, and Jeffrey Pfau
 * 
 */

#include "common/LMException.hpp"
#include "common/UDPPacket.hpp"
#include "UDPSocket.hpp"

#ifdef __WIN32
#include "Winsock2.h"
#else
#include <sys/types.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif


void	UDPSocket::init() {
	if ((fd = ::socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		throw LMException("Failed to create UDP socket");
	}

	int		one = 1;
	setsockopt(fd, SOL_SOCKET, SO_BROADCAST, &one, sizeof(one));
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

bool	UDPSocket::bind(unsigned int portno) {
	struct sockaddr_in	addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(portno);

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

