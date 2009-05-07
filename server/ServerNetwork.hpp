/*
 * server/ServerNetwork.hpp
 *
 * Copyright 2009 - Nathan Partlan, Andrew Ayer, Daniel Schneider, and Jeffrey Pfau
 * 
 */

#ifndef LM_SERVER_SERVERNETWORK_HPP
#define LM_SERVER_SERVERNETWORK_HPP

#include "SDL_net.h"
#include <set>
#include <list>
#include <stdint.h>

class Server;
class PacketWriter;
class RawPacket;

class ServerNetwork {
private:
	UDPsocket		m_socket;
	SDLNet_SocketSet	m_socket_set;
	std::set<int>		m_bound_channels;
	std::list<int>		m_unbound_channels;

	// Process the individual packet which has been received
	void		process_packet (Server& controller, const RawPacket& packet);

	// Send the given raw packet to a specific channel
	void		send_raw_packet(int channel, RawPacket& packet);
public:
	ServerNetwork();
	~ServerNetwork();

	// "Listen" on given port
	//  Returns true if successfully listened, false otherwise
	bool		start(unsigned int portno);
	// Stop listening
	void		stop();

	bool		is_running() const { return m_socket != NULL; }

	// Process all packets and notify the server of their receipt
	// Wait up to the given timeout (in ms) for packets
	// Returns: true if packets were received, false if timeout happened first
	bool		receive_packets(Server& server, uint32_t timeout);
	// Send the given packet to a specific channel
	void		send_packet(int channel, const PacketWriter& packet);
	void		send_packet(int channel, const std::string& packet_data);
	// Send the given packet to a specific address
	void		send_packet(const IPaddress& address, const PacketWriter& packet);
	// Send the given packet to all clients
	// Optionally, you can specify a channel to EXCLUDE from the broadcast
	void		broadcast_packet(const PacketWriter& packet, int exclude_channel =-1);

	// Bind the next available channel to given address
	// Returns -1 if no channels are available
	int		bind(const IPaddress& address);
	// Unbind the specified channel - should be called when clients leave
	void		unbind(int channel);
};

#endif
