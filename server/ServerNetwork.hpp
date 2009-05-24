/*
 * server/ServerNetwork.hpp
 *
 * Copyright 2009 - Nathan Partlan, Andrew Ayer, Daniel Schneider, and Jeffrey Pfau
 * 
 */

#ifndef LM_SERVER_SERVERNETWORK_HPP
#define LM_SERVER_SERVERNETWORK_HPP

#include "common/UDPSocket.hpp"
#include <stdint.h>
#include <string>

class Server;
class PacketWriter;
class UDPPacket;
class IPAddress;

class ServerNetwork {
private:
	UDPSocket	m_socket;

	// Process an individual packet which has been received
	void		process_packet(Server& controller, const UDPPacket& packet);

	// Send the given raw packet
	void		send_raw_packet(const UDPPacket& raw_packet);

public:
	ServerNetwork();
	~ServerNetwork();

	// "Listen" on given port
	//  Returns true if successfully listened, false otherwise
	bool		start(unsigned int portno);

	// TODO: bring back stop() and is_running() functions

	// Process all packets and notify the server of their receipt
	// Wait up to the given timeout (in ms) for packets
	// Returns: true if packets were received, false if timeout happened first
	bool		receive_packets(Server& server, uint64_t timeout);

	// Send the given packet to the given address
	void		send_packet(const IPAddress& address, const PacketWriter& packet);
	void		send_packet(const IPAddress& address, const std::string& packet_data);
};

#endif
