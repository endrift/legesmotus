/*
 * common/CommonNetwork.hpp
 *
 * This file is part of Leges Motus, a networked, 2D shooter set in zero gravity.
 * 
 * Copyright 2009-2010 Andrew Ayer, Nathan Partlan, Jeffrey Pfau
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

#ifndef LM_COMMON_COMMONNETWORK_HPP
#define LM_COMMON_COMMONNETWORK_HPP

#include "UDPSocket.hpp"
#include "AckManager.hpp"
#include "PacketQueue.hpp"
#include <stdint.h>

namespace LM {
	class UDPPacket;
	class IPAddress;
	class PacketReader;
	class PacketWriter;
	class Packet;

	class CommonNetwork {
	public:
		class Peer {
		public:
			uint32_t		connection_id;			// For both sending and receiving packets
			uint64_t		next_sequence_no;		// For sending packets
			PacketQueue		packet_queue;			// For receiving packets

			Peer();
			
			void			init (uint32_t connection_id, uint64_t next_send_sequence_no =1L, uint64_t next_receive_sequence_no =1L);
		};
	protected:
		AckManager	m_ack_manager;
		UDPSocket	m_socket;

		// Send/receive _single_ packets, in raw form.
		void		send_raw_packet(const UDPPacket& raw_packet);
		// Returns true if a packet was received, false if no packets are waiting to be received
		bool		receive_raw_packet(UDPPacket& raw_packet);

		// Send an ACK packet for the given packet
		void		send_ack(const IPAddress& peer, const PacketReader& packet_to_ack);
		void		send_ack(const IPAddress& peer, const Packet& packet_to_ack);
		// Process an ACK packet
		void		process_ack(const IPAddress& peer, PacketReader& ack_packet);
		void		process_ack(const Packet& ack_packet);

	public:
		virtual ~CommonNetwork() { }

		// Send a packet
		void		send_packet(const IPAddress& dest, Packet* packet);
		void		send_packet(const IPAddress& dest, const PacketWriter& packet);
		void		send_packet(const IPAddress& dest, const PacketHeader& header, const std::string& data);

		bool		has_ack_packets() const { return m_ack_manager.has_packets(); }
		uint64_t	time_until_ack_resend() const { return m_ack_manager.time_until_resend(); }
		void		resend_acks();

		virtual void	excessive_packet_drop(const IPAddress& peer) { }
	};
}

#endif
