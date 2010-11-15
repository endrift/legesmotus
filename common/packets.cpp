/*
 * common/packets.cpp
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

#include "packets.hpp"
#include "PacketReceiver.hpp"

using namespace LM;
using namespace std;

void BasePacket::marshal() {
	PacketWriter w(type());
	marshal(w);
	packet.fill(w.packet_data());
}

void BasePacket::unmarshal() {
	PacketReader r(packet);
	// TODO assert r's type == type()
	unmarshal(r);
}

void Packet_ACK::dispatch(PacketReceiver* r) { r->packet_ack(*this); }
void Packet_PLAYER_UPDATE::dispatch(PacketReceiver* r) { r->packet_player_update(*this); }
void Packet_WEAPON_DISCHARGED::dispatch(PacketReceiver* r) { r->packet_weapon_discharged(*this); }
void Packet_PLAYER_HIT::dispatch(PacketReceiver* r) { r->packet_player_hit(*this); }
void Packet_MESSAGE::dispatch(PacketReceiver* r) { r->packet_message(*this); }
void Packet_NEW_ROUND::dispatch(PacketReceiver* r) { r->packet_new_round(*this); }
void Packet_ROUND_OVER::dispatch(PacketReceiver* r) { r->packet_round_over(*this); }
void Packet_SCORE_UPDATE::dispatch(PacketReceiver* r) { r->packet_score_update(*this); }
void Packet_WELCOME::dispatch(PacketReceiver* r) { r->packet_welcome(*this); }
void Packet_ANNOUNCE::dispatch(PacketReceiver* r) { r->packet_announce(*this); }
void Packet_GATE_UPDATE::dispatch(PacketReceiver* r) { r->packet_gate_update(*this); }
void Packet_JOIN::dispatch(PacketReceiver* r) { r->packet_join(*this); }
//void Packet_INFO_server::dispatch(PacketReceiver* r) { r->packet_info_server(*this); }
//void Packet_INFO_client::dispatch(PacketReceiver* r) { r->packet_info_client(*this); }
void Packet_LEAVE::dispatch(PacketReceiver* r) { r->packet_leave(*this); }
void Packet_PLAYER_ANIMATION::dispatch(PacketReceiver* r) { r->packet_player_animation(*this); }
void Packet_REQUEST_DENIED::dispatch(PacketReceiver* r) { r->packet_request_denied(*this); }
void Packet_NAME_CHANGE::dispatch(PacketReceiver* r) { r->packet_name_change(*this); }
void Packet_TEAM_CHANGE::dispatch(PacketReceiver* r) { r->packet_team_change(*this); }
//void Packet_REGISTER_SERVER_server::dispatch(PacketReceiver* r) { r->packet_register_server_server(*this); }
//void Packet_REGISTER_SERVER_metaserver::dispatch(PacketReceiver* r) { r->packet_register_server_metaserver(*this); }	
void Packet_UNREGISTER_SERVER::dispatch(PacketReceiver* r) { r->packet_unregister_server(*this); }
void Packet_UPGRADE_AVAILABLE::dispatch(PacketReceiver* r) { r->packet_upgrade_available(*this); }
void Packet_MAP_INFO::dispatch(PacketReceiver* r) { r->packet_map_info(*this); }
void Packet_MAP_OBJECT::dispatch(PacketReceiver* r) { r->packet_map_object(*this); }
void Packet_GAME_PARAM::dispatch(PacketReceiver* r) { r->packet_game_param(*this); }
void Packet_HOLE_PUNCH::dispatch(PacketReceiver* r) { r->packet_hole_punch(*this); }
void Packet_PLAYER_DIED::dispatch(PacketReceiver* r) { r->packet_player_died(*this); }
void Packet_WEAPON_INFO::dispatch(PacketReceiver* r) { r->packet_weapon_info(*this); }
void Packet_ROUND_START::dispatch(PacketReceiver* r) { r->packet_round_start(*this); }
void Packet_SPAWN::dispatch(PacketReceiver* r) { r->packet_spawn(*this); }
