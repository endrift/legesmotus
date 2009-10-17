/*
 * common/AckManager.hpp
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

#ifndef LM_COMMON_ACKMANAGER_HPP
#define LM_COMMON_ACKMANAGER_HPP

#include <string>
#include <list>
#include <map>
#include <set>
#include <stdint.h>

namespace LM {
	class PacketWriter;
	
	class AckManager {
		enum {
			ACK_TIME = 1000,
			RETRIES = 5
		};
	protected:
		struct SentPacket {
			uint64_t		send_time;
			std::set<uint32_t>	peer_ids;
			uint32_t		packet_type;
			uint32_t		packet_id;
			std::string		data;
			int			tries_left;
	
			SentPacket(uint32_t peer_id, const PacketWriter& packet);
			SentPacket(const std::set<uint32_t>& peer_ids, const PacketWriter& packet);
	
			void			ack(uint32_t peer_id);
			uint64_t		time_since_send() const;
			uint64_t		time_until_resend() const;
			void			reset_send_time();
			bool			has_peers() const { return !peer_ids.empty(); }
		};
		friend class SentPacket;
	
		virtual void	kick_peer(uint32_t peer_id) = 0;
		virtual void	resend_packet(uint32_t peer_id, const std::string& data) = 0;
	
	private:
		typedef std::list<SentPacket> Queue;
	
		Queue					m_packets;
		std::map<uint32_t, Queue::iterator>	m_packets_by_id;
	
	public:
		virtual ~AckManager() { }
	
		void		add_packet(uint32_t peer_id, const PacketWriter& packet);
		void		add_broadcast_packet(const std::set<uint32_t>& peer_ids, const PacketWriter& packet);
		void		ack(uint32_t peer_id, uint32_t packet_id);
		bool		has_packets() const { return !m_packets.empty(); }
		uint64_t	time_until_resend() const;
		void		resend();
	};
}

#endif
