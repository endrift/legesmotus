/*
 * ClientNetwork.hpp
 *
 * Copyright 2009 - Nathan Partlan, Andrew Ayer, Daniel Schneider, and Jeffrey Pfau
 * 
 */
 
#ifndef LM_CLIENT_CLIENTNETWORK_HPP
#define LM_CLIENT_CLIENTNETWORK_HPP

#include "SDL_net.h"

class GameController;
class PacketWriter;
class RawPacket;

class ClientNetwork {
private:
	UDPsocket	m_socket;		// SDL socket to the server, or NULL if not connected
	IPaddress	m_server_address;	// The resolved address of the server (host and port)
	int		m_server_channel;	// The channel to which the server is bound

	// Process the individual packet which has been received
	void		process_packet (GameController& controller, const RawPacket& packet);

	// Send an acknowledgment packet for given packet id back to the server
	void		send_ack (uint32_t packet_id);
	
public:
	ClientNetwork();
	~ClientNetwork();

	// Connect to give hostname on given port
	//  Returns true if successfully connected, false otherwise
	//  Does not send join packet
	bool		connect(const char* hostname, unsigned int portno);
	// Disconnect from server (Does not send leave packet)
	void		disconnect();

	bool		is_connected() const { return m_socket != NULL; }

	// Process all the pending packets and notify the game controller of their receipt
	void		receive_packets(GameController& game_controller);
	// Send the given packet to the server
	void		send_packet(const PacketWriter& packet);

};

#endif

