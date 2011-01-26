/*
 * common/AckManager.hpp
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

#ifndef LM_COMMON_ACKMANAGER_HPP
#define LM_COMMON_ACKMANAGER_HPP

#include <string>
#include <list>
#include <map>
#include <set>
#include <stdint.h>
#include "IPAddress.hpp"
#include "PacketHeader.hpp"
#include "Packet.hpp"

namespace LM {
	class PacketWriter;
	class CommonNetwork;
	
	class AckManager {
	private:
		// Parameters controlling ACKs
		enum {
			ACK_TIME = 500,
			RETRIES = 10
		};

		// This class represents a packet which has been sent (possibly to multiple recipients), and is awaiting ACKs
		// Note that when the packet is sent to multiple recipients, all recipients receive the same packet data, but
		// each recipient is sent a different packet header. That's why the packet header is stored in the recipients map.
		class SentPacket {
		private:
			uint64_t				send_time;	// When was it sent?
			std::map<IPAddress, PacketHeader>	recipients;	// Who was it sent to? (and with what header?)
			std::string				packet_data;	// What it was
			int					tries_left;	// How many more times to try sending it
	
			// Use this constructor to construct a broadcast packet.
			// Then call add_recipient() to add the peers to which it was sent.
			explicit SentPacket(const std::string& packet_data);

			// Use this function to construct a unicast packet
			// It's just shorthand for SentPacket(data) followed by a call to add_recipient(peer_addr, header)
			SentPacket(const IPAddress& peer_addr, const PacketHeader& header, const std::string& data);
	
			// Add a recipient
			void			add_recipient(const IPAddress&, const PacketHeader&);

			// Call when an ACK is received for this packet for the given recipient
			// It will remove the recipient from this SentPacket
			void			ack(const IPAddress& peer);

			uint64_t		time_since_send() const;	// How long since packet was sent?
			uint64_t		time_until_resend() const;	// How long until we should try resending?

			void			reset_send_time();		// Call when the packet has been resent

			// When has_recipients() returns false, this SentPacket should be removed
			// from the AckManager because all its recipients have received the packet OK
			bool			has_recipients() const { return !recipients.empty(); }

			friend class AckManager;
		};
		friend class SentPacket;
	
		typedef std::list<SentPacket> Queue;
		typedef std::map<std::pair<IPAddress, uint64_t>, Queue::iterator> Map;
	
		// Packets are added to this queue in the order that they are sent
		Queue								m_packets;
		// The map allows quick lookup of a packet by IP address and sequence number
		Map								m_packets_by_id;
	
	public:
		void		clear();
		void		clear_peer(const IPAddress& peer); // Remove all packets for this peer from the ACK manager

		typedef Queue::iterator PacketHandle;

		// add_packet adds a unicast packet to the AckManager
		void		add_packet(const IPAddress& peer_addr, const PacketHeader& header, const std::string& data);
		void		add_packet(const IPAddress& peer_addr, const Packet& packet);

		// Add_broadcast_packet adds a broadcast packet to the AckManager
		// It returns an opaque PacketHandle object.
		// Add each recipient of the broadcast packet by calling add_broadcast_recipient with the PacketHandle object.
		PacketHandle	add_broadcast_packet(const std::string& data);
		PacketHandle	add_broadcast_packet(const Packet& packet);
		void		add_broadcast_recipient(PacketHandle, const IPAddress& peer_addr, const PacketHeader& header);

		// Call ack() when an ACK is received from the given peer for the given sequence number
		void		ack(const IPAddress& peer_addr, uint64_t sequence_no);

		// after time_until_resend() milliseconds elapses, call resend() to resend the packets
		uint64_t	time_until_resend() const;
		void		resend(CommonNetwork& network);

		// if has_packets() returns false, there's no need to periodically call resend()
		bool		has_packets() const { return !m_packets.empty(); }
	};
}

#endif
