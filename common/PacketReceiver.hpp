/*
 * common/PacketReceiver.hpp
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

#ifndef LM_COMMON_PACKETRECEIVER_HPP
#define LM_COMMON_PACKETRECEIVER_HPP

#include "packets.hpp"

namespace LM {
	class PacketReceiver {
	public:
		virtual ~PacketReceiver() { }

		void receive_packet(const UDPPacket& packet);

		virtual void packet_ack(const Packet_ACK& p) { }
		virtual void packet_player_update(const Packet_PLAYER_UPDATE& p) { }
		virtual void packet_weapon_discharged(const Packet_WEAPON_DISCHARGED& p) { }
		virtual void packet_player_hit(const Packet_PLAYER_HIT& p) { }
		virtual void packet_message(const Packet_MESSAGE& p) { }
		virtual void packet_new_round(const Packet_NEW_ROUND& p) { }
		virtual void packet_round_over(const Packet_ROUND_OVER& p) { }
		virtual void packet_score_update(const Packet_SCORE_UPDATE& p) { }
		virtual void packet_welcome(const Packet_WELCOME& p) { }
		virtual void packet_announce(const Packet_ANNOUNCE& p) { }
		virtual void packet_gate_update(const Packet_GATE_UPDATE& p) { }
		virtual void packet_join(const Packet_JOIN& p) { }
		//virtual void packet_info(const Packet_INFO& p) { }
		virtual void packet_leave(const Packet_LEAVE& p) { }
		virtual void packet_player_animation(const Packet_PLAYER_ANIMATION& p) { }
		virtual void packet_request_denied(const Packet_REQUEST_DENIED& p) { }
		virtual void packet_name_change(const Packet_NAME_CHANGE& p) { }
		virtual void packet_team_change(const Packet_TEAM_CHANGE& p) { }
		//virtual void packet_register_server(const Packet_REGISTER_SERVER& p) { }
		virtual void packet_unregister_server(const Packet_UNREGISTER_SERVER& p) { }
		virtual void packet_upgrade_available(const Packet_UPGRADE_AVAILABLE& p) { }
		virtual void packet_map_info(const Packet_MAP_INFO& p) { }
		virtual void packet_map_object(const Packet_MAP_OBJECT& p) { }
		virtual void packet_game_param(const Packet_GAME_PARAM& p) { }
		virtual void packet_hole_punch(const Packet_HOLE_PUNCH& p) { }
		virtual void packet_player_died(const Packet_PLAYER_DIED& p) { }
		virtual void packet_weapon_info(const Packet_WEAPON_INFO& p) { }
		virtual void packet_round_start(const Packet_ROUND_START& p) { }
		virtual void packet_spawn(const Packet_SPAWN& p) { }
	};
}

#endif
