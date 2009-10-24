/*
 * server/ServerNetwork.hpp
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

#ifndef LM_SERVER_SERVERNETWORK_HPP
#define LM_SERVER_SERVERNETWORK_HPP

#include "common/CommonNetwork.hpp"
#include "common/PacketWriter.hpp"
#include <stdint.h>
#include <string>
#include <map>

namespace LM {
	class Server;
	class PacketWriter;
	class UDPPacket;
	class IPAddress;
	
	class ServerNetwork : public CommonNetwork {
	private:
		// A reference to our owning Server object
		Server&		m_server;

		// Map of addresses to their NetworkPeer objects
		std::map<IPAddress, Peer>	m_peers;
		Peer*	get_peer(const IPAddress&); // Convenience function to lookup in m_peers map

		// Process an individual packet which has been received
		void		process_packet(const IPAddress& peer_address, PacketReader& packet);

		virtual void	excessive_packet_drop(const IPAddress& peer);
	
	public:
		explicit ServerNetwork(Server& s) : m_server(s) { }

		// "Listen" on given address
		//  Returns true if successfully listened, false otherwise
		bool		start(const IPAddress& address);
	
		// TODO: bring back stop() and is_running() functions


		/*
		 * Peer functions
		 */
		void		register_peer(const IPAddress&, uint32_t connection_id, uint64_t next_send_sequence_no, uint64_t next_receive_sequence_no);
		void		unregister_peer(const IPAddress&);


		/*
		 * Receiving packets
		 */
	
		// Process all packets and notify the server of their receipt
		// Wait up to the given timeout (in ms) for packets
		// Returns: true if packets were received, false if timeout or signal happened first
		bool		receive_packets(uint32_t timeout);


		/*
		 * Sending packets
		 */
	
		// Send the given packet to the given address
		void		send_reliable_packet(const IPAddress& address, const PacketWriter& packet);

		using CommonNetwork::send_packet;

		// Broadcast the address to all registered peers (except perhaps a certain peer, if not NULL)
		void		broadcast_reliable_packet(const PacketWriter& packet, const IPAddress* exclude_peer =NULL);
		void		broadcast_packet(const PacketWriter& packet, const IPAddress* exclude_peer =NULL);
	};
}

#endif
