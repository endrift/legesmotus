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

class Server;
class PacketWriter;
class RawPacket;

class ServerNetwork {
private:
	UDPsocket		m_socket;
	SDLNet_SocketSet	m_socket_set;
	std::set<int>		m_bound_channels;
	int			m_next_unbound_channel;

	// Process the individual packet which has been received
	void		process_packet (Server& controller, const RawPacket& packet);

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
	// Wait up to the given timeout (in ms) for packets (-1 == wait forever)
	// Returns: true if packets were received, false if timeout happened first
	bool		receive_packets(Server& server, long timeout);
	// Send the given packet to a specific channel
	void		send_packet(int channel, const PacketWriter& packet);
	// Send the given packet to all clients
	// Optionally, you can specify a channel to EXCLUDE from the broadcast
	void		broadcast_packet(const PacketWriter& packet, int exclude_channel =-1);

	// Unbind the specified channel - should be called when clients leave
	void		unbind(int channel);
};

#endif
