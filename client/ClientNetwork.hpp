/*
 * ClientNetwork.hpp
 *
 * Copyright 2009 - Nathan Partlan, Andrew Ayer, Daniel Schneider, and Jeffrey Pfau
 * 
 */
 
#ifndef LM_CLIENT_CLIENTNETWORK_HPP
#define LM_CLIENT_CLIENTNETWORK_HPP

#include "SDL_net.h"
#include <stdint.h>

class GameController;
class PacketWriter;
class RawPacket;

class ClientNetwork {
private:
	UDPsocket	m_socket;		// SDL socket to the server, or NULL if not connected
	IPaddress	m_server_address;	// The resolved address of the server (host and port)
	int		m_server_channel;	// The channel to which the server is bound

	// Process the individual packet which has been received.
	// Packets from our bound server
	void		process_bound_packet(GameController& controller, const RawPacket& packet);
	// Packets from other sources (e.g. for server browsing)
	void		process_unbound_packet(GameController& controller, const RawPacket& packet);

	void		send_raw_packet(int channel, RawPacket& raw_packet);

public:
	ClientNetwork();
	~ClientNetwork();

	// Connect to give hostname on given port
	//  Returns true if successfully connected, false otherwise
	//  Does not send join packet
	bool		connect(const char* hostname, unsigned int portno);
	// Disconnect from server (Does not send leave packet)
	void		disconnect();

	bool		is_connected() const { return m_server_channel != -1; }

	// Process all the pending packets and notify the game controller of their receipt
	void		receive_packets(GameController& game_controller);
	// Send the given packet to the server
	void		send_packet(const PacketWriter& packet);

	// Send the given packet to the specific address
	void		send_unbound_packet(const IPaddress& dest, const PacketWriter& packet);

	// The following two functions are for internal use only.  They are public in order to facilitate testing.
	// Use receive_packets and send_packet instead!

	// Receive a _single_ packet, in raw form.  You should use receive_packets instead.
	// Returns true if a packet was received, false if no packets waiting to be received
	bool		receive_raw_packet(RawPacket& raw_packet);
	// Send a packet, in raw form.  Primarily for testing.  You should use send_packet instead.
	void		send_raw_packet(RawPacket& raw_packet);


};

#endif

