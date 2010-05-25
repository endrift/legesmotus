/*
 * ServerScannerNetwork.hpp
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
 
#ifndef LM_CLIENT_SERVERSCANNERNETWORK_HPP
#define LM_CLIENT_SERVERSCANNERNETWORK_HPP

#include "common/IPAddress.hpp"
#include "common/CommonNetwork.hpp"
#include "common/PacketWriter.hpp"
#include <stdint.h>

namespace LM {
	class ServerScanner;
	class PacketHeader;
	class UDPPacket;
	
	class ServerScannerNetwork : public CommonNetwork {
	private:
		static uint32_t	next_connection_id;

		ServerScanner&	m_controller;		// Our owning Game Controller, which we inform of packet receipts
		IPAddress	m_server_address;	// The resolved address of the server (host & port)
		Peer		m_server_peer;		// Details about the server connection
		bool		m_is_connected;		// Are we currently connected to the server?
		uint64_t	m_last_packet_time;	// The time we last received a packet from the server
	
		// Packets from other sources (e.g. for server browsing)
		void		process_unbound_packet(const UDPPacket& packet);

	public:
		explicit ServerScannerNetwork(ServerScanner& gc);
	
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
		void		receive_packets(uint32_t timeout);
	
	
		/*
		 * Sending packets
		 */
	
		// Send the given packet to the server
		void		send_reliable_packet(const PacketWriter& packet);
		void		send_packet(const PacketWriter& packet) { send_packet(packet.get_header(), packet.packet_data()); }
		void		send_packet(const PacketHeader& header, const std::string& data);
	
		// Send the given packet to the specific address
		void		send_packet_to(const IPAddress& dest, const PacketWriter& packet) { CommonNetwork::send_packet(dest, packet); }
		void		send_packet_to(const IPAddress& dest, const PacketHeader& header, const std::string& data) { CommonNetwork::send_packet(dest, header, data); }
	
		// Send the given packet to all systems on the LAN
		void		broadcast_packet(unsigned int portno, const PacketWriter& packet);
	};
}

#endif

