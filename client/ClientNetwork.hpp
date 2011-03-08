/*
 * client/ClientNetwork.hpp
 *
 * This file is part of Leges Motus, a networked, 2D shooter set in zero gravity.
 * 
 * Copyright 2009-2011 Andrew Ayer, Nathan Partlan, Jeffrey Pfau
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

#ifndef LM_CLIENT_CLIENTNETWORK_HPP
#define LM_CLIENT_CLIENTNETWORK_HPP

#include "common/IPAddress.hpp"
#include "common/CommonNetwork.hpp"
#include "common/Packet.hpp"
#include <stdint.h>

namespace LM {
	class PacketHeader;
	class UDPPacket;
	
	class ClientNetwork : public CommonNetwork {
	private:
		static uint32_t	next_connection_id;

		PacketReceiver*	m_client;       // Our owning client, which we inform of packet receipts
		IPAddress	m_server_address;   // The resolved address of the server (host & port)
		Peer		m_server_peer;      // Details about the server connection
		bool		m_is_connected;     // Are we currently connected to the server?
		uint64_t	m_last_packet_time; // The time we last received a packet from the server
	
		virtual void	excessive_packet_drop(const IPAddress& peer);

	public:
		explicit ClientNetwork(PacketReceiver* client);
	
		/*
		 * Connection functions - for connecting and disconnecting with the server
		 */
	
		// Connect to given hostname on given port
		//  Returns true if successfully connected, false otherwise
		//  Does not send join packet
		bool		connect(const char* hostname, unsigned int portno);
		bool		connect(const IPAddress& address);
	
		// Disconnect from server (Does not send leave packet)
		void		disconnect();
	
		// Are we currently connected with a server?
		bool		is_connected() const { return m_is_connected; }
	
		const IPAddress& get_server_address() const { return m_server_address; }
	
		// Return the last tick on which a packet was received from the server.
		uint64_t	get_last_packet_time();
	
	
		/*
		 * Receiving packets
		 */
	
		// Process all the pending packets and notify the game controller of their receipt
		void		receive_packets();
	
	
		/*
		 * Sending packets
		 */
	
		// Send the given packet to the server
		void		send_reliable_packet(Packet* packet);
		void		send_packet(Packet* packet);
	
		// Send the given packet to the specific address
		void		send_packet_to(const IPAddress& dest, Packet* packet) { CommonNetwork::send_packet(dest, packet); }
	
		// Send the given packet to all systems on the LAN
		void		broadcast_packet(unsigned int portno, Packet* packet);
	};
}

#endif
