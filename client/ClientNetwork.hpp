/*
 * ClientNetwork.hpp
 *
 * Copyright 2009 - Nathan Partlan, Andrew Ayer, Daniel Schneider, and Jeffrey Pfau
 * 
 */
 
#ifndef LM_CLIENT_CLIENTNETWORK_HPP
#define LM_CLIENT_CLIENTNETWORK_HPP

#include "common/IPAddress.hpp"
#include "common/UDPSocket.hpp"
#include <stdint.h>

class GameController;
class PacketWriter;
class UDPPacket;

class ClientNetwork {
private:
	UDPSocket	m_socket;		// The socket we use for UDP communication
	IPAddress	m_server_address;	// The resolved address of the server (host & port)
	bool		m_is_connected;		// Are we currently connected to the server?

	// Process the individual packet which has been received.
	// Packets from our bound server
	void		process_server_packet(GameController& controller, const UDPPacket& packet);
	// Packets from other sources (e.g. for server browsing)
	void		process_unbound_packet(GameController& controller, const UDPPacket& packet);

public:
	ClientNetwork();
	~ClientNetwork();

	/*
	 * Connection functions - for connecting and disconnecting with the server
	 */

	// Connect to given hostname on given port
	//  Returns true if successfully connected, false otherwise
	//  Does not send join packet
	bool		connect(const char* hostname, unsigned int portno);

	// Disconnect from server (Does not send leave packet)
	void		disconnect();

	// Are we currently connected with a server?
	bool		is_connected() const { return m_is_connected; }

	const IPAddress& get_server_address() const { return m_server_address; }


	/*
	 * Receiving packets
	 */

	// Process all the pending packets and notify the game controller of their receipt
	void		receive_packets(GameController& game_controller);


	/*
	 * Sending packets
	 */

	// Send the given packet to the server
	void		send_packet(const PacketWriter& packet);

	// Send the given packet to the specific address
	void		send_packet_to(const IPAddress& dest, const PacketWriter& packet);

	// Send the given packet to all systems on the LAN
	void		broadcast_packet(unsigned int portno, const PacketWriter& packet);


	/*
	 * Specialized communication functions - for sending and receiving RAW packets
	 *  Primarily for internal use, but are made public for TESTING PURPOSES ONLY
	 *  Use receive_packets and send_packet instead!
	 */

	// Receive a _single_ packet, in raw form.  You should use receive_packets instead.
	// Returns true if a packet was received, false if no packets waiting to be received
	bool		receive_raw_packet(UDPPacket& raw_packet);
	// Send a packet, in raw form.  You should not use this function; use send_packet instead.
	void		send_raw_packet(const UDPPacket& raw_packet);

};

#endif

