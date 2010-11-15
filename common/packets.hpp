/*
 * common/packets.hpp
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

#ifndef LM_COMMON_PACKETS_HPP
#define LM_COMMON_PACKETS_HPP

#include "PacketReader.hpp"
#include "PacketWriter.hpp"
#include "Version.hpp"
#include "Point.hpp"
#include "IPAddress.hpp"
#include "UDPPacket.hpp"
#include <string>

namespace LM {
	class PacketReceiver;

	struct BasePacket {
		UDPPacket packet;

		virtual ~BasePacket() {}
		virtual PacketType type() = 0;
		virtual void marshal(PacketWriter& w) = 0;
		virtual void unmarshal(PacketReader& r) = 0;

		void marshal();
		void unmarshal();

		virtual void dispatch(PacketReceiver* r) = 0;
	};

	struct Packet_ACK : public BasePacket {
		virtual ~Packet_ACK() {}
		PacketType type() { return ACK_PACKET; }

		uint32_t packet_type;
		uint64_t sequence_no;

		void marshal(PacketWriter& w) {
			w << packet_type;
			w << sequence_no;
		}

		void unmarshal(PacketReader& r) {
			r >> packet_type;
			r >> sequence_no;
		}
		void dispatch(PacketReceiver* r);
	};

	struct Packet_PLAYER_UPDATE : public BasePacket {
		virtual ~Packet_PLAYER_UPDATE() {}
		PacketType type() { return PLAYER_UPDATE_PACKET; }

		uint32_t player_id;
		float x;
		float y;
		float x_vel;
		float y_vel;
		float rotation;
		int energy;
		std::string current_weapon_id;
		std::string flags;

		void marshal(PacketWriter& w) {
			w << player_id;
			w << x;
			w << y;
			w << x_vel;
			w << y_vel;
			w << rotation;
			w << energy;
			w << current_weapon_id;
			w << flags;
		}

		void unmarshal(PacketReader& r) {
			r >> player_id;
			r >> x;
			r >> y;
			r >> x_vel;
			r >> y_vel;
			r >> rotation;
			r >> energy;
			r >> current_weapon_id;
			r >> flags;
		}
		void dispatch(PacketReceiver* r);
	};

	struct Packet_WEAPON_DISCHARGED : public BasePacket {
		virtual ~Packet_WEAPON_DISCHARGED() {}
		PacketType type() { return WEAPON_DISCHARGED_PACKET; }

		uint32_t player_id;
		std::string weapon_id;
		std::string extradata;

		void marshal(PacketWriter& w) {
			w << player_id;
			w << weapon_id;
			w << extradata;
		}

		void unmarshal(PacketReader& r) {
			r >> player_id;
			r >> weapon_id;
			r >> extradata;
		}
		void dispatch(PacketReceiver* r);
	};

	struct Packet_PLAYER_HIT : public BasePacket {
		virtual ~Packet_PLAYER_HIT() {}
		PacketType type() { return PLAYER_HIT_PACKET; }

		uint32_t shooter_id;
		std::string weapon_id;
		uint32_t shot_player_id;
		bool has_effect;
		std::string extradata;

		void marshal(PacketWriter& w) {
			w << shooter_id;
			w << weapon_id;
			w << shot_player_id;
			w << has_effect;
			w << extradata;
		}

		void unmarshal(PacketReader& r) {
			r >> shooter_id;
			r >> weapon_id;
			r >> shot_player_id;
			r >> has_effect;
			r >> extradata;
		}
		void dispatch(PacketReceiver* r);
	};

	struct Packet_MESSAGE : public BasePacket {
		virtual ~Packet_MESSAGE() {}
		PacketType type() { return MESSAGE_PACKET; }

		uint32_t sender_id;
		std::string recipient;
		std::string message_text;

		void marshal(PacketWriter& w) {
			w << sender_id;
			w << recipient;
			w << message_text;
		}

		void unmarshal(PacketReader& r) {
			r >> sender_id;
			r >> recipient;
			r >> message_text;
		}
		void dispatch(PacketReceiver* r);
	};

	struct Packet_NEW_ROUND : public BasePacket {
		virtual ~Packet_NEW_ROUND() {}
		PacketType type() { return NEW_ROUND_PACKET; }

		std::string map_name;
		int map_revision;
		int map_width;
		int map_height;
		bool game_started;
		uint64_t time_until_start;

		void marshal(PacketWriter& w) {
			w << map_name;
			w << map_revision;
			w << map_width;
			w << map_height;
			w << game_started;
			w << time_until_start;
		}

		void unmarshal(PacketReader& r) {
			r >> map_name;
			r >> map_revision;
			r >> map_width;
			r >> map_height;
			r >> game_started;
			r >> time_until_start;
		}
		void dispatch(PacketReceiver* r);
	};

	struct Packet_ROUND_OVER : public BasePacket {
		virtual ~Packet_ROUND_OVER() {}
		PacketType type() { return ROUND_OVER_PACKET; }

		char winning_team;
		int team_a_score;
		int team_b_score;

		void marshal(PacketWriter& w) {
			w << winning_team;
			w << team_a_score;
			w << team_b_score;
		}

		void unmarshal(PacketReader& r) {
			r >> winning_team;
			r >> team_a_score;
			r >> team_b_score;
		}
		void dispatch(PacketReceiver* r);
	};

	struct Packet_SCORE_UPDATE : public BasePacket {
		virtual ~Packet_SCORE_UPDATE() {}
		PacketType type() { return SCORE_UPDATE_PACKET; }

		std::string subject;
		int score;

		void marshal(PacketWriter& w) {
			w << subject;
			w << score;
		}

		void unmarshal(PacketReader& r) {
			r >> subject;
			r >> score;
		}
		void dispatch(PacketReceiver* r);
	};

	struct Packet_WELCOME : public BasePacket {
		virtual ~Packet_WELCOME() {}
		PacketType type() { return WELCOME_PACKET; }

		int server_version;
		int player_id;
		std::string player_name;
		char team;

		void marshal(PacketWriter& w) {
			w << server_version;
			w << player_id;
			w << player_name;
			w << team;
		}

		void unmarshal(PacketReader& r) {
			r >> server_version;
			r >> player_id;
			r >> player_name;
			r >> team;
		}
		void dispatch(PacketReceiver* r);
	};

	struct Packet_ANNOUNCE : public BasePacket {
		virtual ~Packet_ANNOUNCE() {}
		PacketType type() { return ANNOUNCE_PACKET; }

		uint32_t player_id;
		std::string player_name;
		char team;

		void marshal(PacketWriter& w) {
			w << player_id;
			w << player_name;
			w << team;
		}

		void unmarshal(PacketReader& r) {
			r >> player_id;
			r >> player_name;
			r >> team;
		}
		void dispatch(PacketReceiver* r);
	};

	struct Packet_GATE_UPDATE : public BasePacket {
		virtual ~Packet_GATE_UPDATE() {}
		PacketType type() { return GATE_UPDATE_PACKET; }

		uint32_t acting_player_id;
		char team;
		float progress;
		int change_in_players;
		size_t new_nbr_players;
		uint64_t sequence_no;

		void marshal(PacketWriter& w) {
			w << acting_player_id;
			w << team;
			w << progress;
			w << change_in_players;
			w << new_nbr_players;
			w << sequence_no;
		}

		void unmarshal(PacketReader& r) {
			r >> acting_player_id;
			r >> team;
			r >> progress;
			r >> change_in_players;
			r >> new_nbr_players;
			r >> sequence_no;
		}
		void dispatch(PacketReceiver* r);
	};

	struct Packet_JOIN : public BasePacket {
		virtual ~Packet_JOIN() {}
		PacketType type() { return JOIN_PACKET; }

		int protocol_number;
		Version compat_version;
		std::string name;
		char team;

		void marshal(PacketWriter& w) {
			w << protocol_number;
			w << compat_version;
			w << name;
			w << team;
		}

		void unmarshal(PacketReader& r) {
			r >> protocol_number;
			r >> compat_version;
			r >> name;
			r >> team;
		}
		void dispatch(PacketReceiver* r);
	};

	// TODO fill in the rest

	struct Packet_INFO_server : public BasePacket {
		virtual ~Packet_INFO_server() {}
		PacketType type() { return INFO_PACKET; }

		uint32_t request_packet_id;
		uint64_t scan_start_time;
		IPAddress server_address;
		int server_protocol_version;
		Version server_compat_version;
		std::string current_map_name;
		int team_count[2];
		int max_players;
		uint64_t uptime;
		uint64_t time_left_in_game;
		std::string server_name;
		std::string server_location;

		void marshal(PacketWriter& w) {}

		void unmarshal(PacketReader& r) {}
		void dispatch(PacketReceiver* r);
	};

	struct Packet_INFO_client : public BasePacket {
		virtual ~Packet_INFO_client() {}
		PacketType type() { return INFO_PACKET; }

		int client_proto_version;
		uint32_t scan_id;
		uint64_t scan_start_time;
		Version client_version;

		void marshal(PacketWriter& w) {}
		void unmarshal(PacketReader& r) {}
		void dispatch(PacketReceiver* r);
	};

	struct Packet_LEAVE : public BasePacket {
		virtual ~Packet_LEAVE() {}
		PacketType type() { return LEAVE_PACKET; }

		uint32_t player_id;
		std::string message;

		void marshal(PacketWriter& w) {}
		void unmarshal(PacketReader& r) {}
		void dispatch(PacketReceiver* r);
	};

	struct Packet_PLAYER_ANIMATION : public BasePacket {
		virtual ~Packet_PLAYER_ANIMATION() {}
		PacketType type() { return PLAYER_ANIMATION_PACKET; }

		uint32_t player_id;
		std::string sprite_list;
		std::string field;
		int value;

		void marshal(PacketWriter& w) {}
		void unmarshal(PacketReader& r) {}
		void dispatch(PacketReceiver* r);
	};

	struct Packet_REQUEST_DENIED : public BasePacket {
		virtual ~Packet_REQUEST_DENIED() {}
		PacketType type() { return REQUEST_DENIED_PACKET; }

		int packet_type;
		std::string message;

		void marshal(PacketWriter& w) {}
		void unmarshal(PacketReader& r) {}
		void dispatch(PacketReceiver* r);
	};

	struct Packet_NAME_CHANGE : public BasePacket {
		virtual ~Packet_NAME_CHANGE() {}
		PacketType type() { return NAME_CHANGE_PACKET; }

		uint32_t player_id;
		std::string name;

		void marshal(PacketWriter& w) {}
		void unmarshal(PacketReader& r) {}
		void dispatch(PacketReceiver* r);
	};

	struct Packet_TEAM_CHANGE : public BasePacket {
		virtual ~Packet_TEAM_CHANGE() {}
		PacketType type() { return TEAM_CHANGE_PACKET; }

		uint32_t player_id;
		char name;

		void marshal(PacketWriter& w) {}
		void unmarshal(PacketReader& r) {}
		void dispatch(PacketReceiver* r);
	};

	struct Packet_REGISTER_SERVER_server : public BasePacket {
		virtual ~Packet_REGISTER_SERVER_server() {}
		PacketType type() { return REGISTER_SERVER_PACKET; }

		int server_protocol_version;
		Version server_version;
		IPAddress server_listen_address;

		void marshal(PacketWriter& w) {}
		void unmarshal(PacketReader& r) {}
		void dispatch(PacketReceiver* r);
	};

	struct Packet_REGISTER_SERVER_metaserver : public BasePacket {
		virtual ~Packet_REGISTER_SERVER_metaserver() {}
		PacketType type() { return REGISTER_SERVER_PACKET; }

		uint32_t metaserver_token;
		uint32_t metaserver_contact_frequency;

		void marshal(PacketWriter& w) {}
		void unmarshal(PacketReader& r) {}
		void dispatch(PacketReceiver* r);
	};	

	struct Packet_UNREGISTER_SERVER : public BasePacket {
		virtual ~Packet_UNREGISTER_SERVER() {}
		PacketType type() { return UNREGISTER_SERVER_PACKET; }

		IPAddress server_listen_address;
		uint32_t metaserver_token;

		void marshal(PacketWriter& w) {}
		void unmarshal(PacketReader& r) {}
		void dispatch(PacketReceiver* r);
	};

	struct Packet_UPGRADE_AVAILABLE : public BasePacket {
		virtual ~Packet_UPGRADE_AVAILABLE() {}
		PacketType type() { return UPGRADE_AVAILABLE_PACKET; }

		std::string latest_version;

		void marshal(PacketWriter& w) {}
		void unmarshal(PacketReader& r) {}
		void dispatch(PacketReceiver* r);
	};

	struct Packet_MAP_INFO : public BasePacket {
		virtual ~Packet_MAP_INFO() {}
		PacketType type() { return MAP_INFO_PACKET; }

		uint32_t transmission_id;
		//Map map;
		size_t num_expected_objects;

		void marshal(PacketWriter& w) {}
		void unmarshal(PacketReader& r) {}
		void dispatch(PacketReceiver* r);
	};

	struct Packet_MAP_OBJECT : public BasePacket {
		virtual ~Packet_MAP_OBJECT() {}
		PacketType type() { return MAP_OBJECT_PACKET; }

		uint32_t transmission_id;
		//MapReceiver object;

		void marshal(PacketWriter& w) {}
		void unmarshal(PacketReader& r) {}
		void dispatch(PacketReceiver* r);
	};

	struct Packet_GAME_PARAM : public BasePacket {
		virtual ~Packet_GAME_PARAM() {}
		PacketType type() { return GAME_PARAM_PACKET; }

		std::string param_name;
		//extra data;

		void marshal(PacketWriter& w) {}
		void unmarshal(PacketReader& r) {}
		void dispatch(PacketReceiver* r);
	};

	struct Packet_HOLE_PUNCH : public BasePacket {
		virtual ~Packet_HOLE_PUNCH() {}
		PacketType type() { return HOLE_PUNCH_PACKET; }

		IPAddress client_address;
		uint32_t scan_id;

		void marshal(PacketWriter& w) {}
		void unmarshal(PacketReader& r) {}
		void dispatch(PacketReceiver* r);
	};

	struct Packet_PLAYER_DIED : public BasePacket {
		virtual ~Packet_PLAYER_DIED() {}
		PacketType type() { return PLAYER_DIED_PACKET; }

		uint32_t killed_player_id;
		uint32_t scan_id;

		void marshal(PacketWriter& w) {}
		void unmarshal(PacketReader& r) {}
		void dispatch(PacketReceiver* r);
	};

	struct Packet_WEAPON_INFO : public BasePacket {
		virtual ~Packet_WEAPON_INFO() {}
		PacketType type() { return WEAPON_INFO_PACKET; }

		size_t index;
		//WeaponReader weapon_data;

		void marshal(PacketWriter& w) {}
		void unmarshal(PacketReader& r) {}
		void dispatch(PacketReceiver* r);
	};

	struct Packet_ROUND_START : public BasePacket {
		virtual ~Packet_ROUND_START() {}
		PacketType type() { return ROUND_START_PACKET; }

		uint64_t time_left_in_round;

		void marshal(PacketWriter& w) {}
		void unmarshal(PacketReader& r) {}
		void dispatch(PacketReceiver* r);
	};

	struct Packet_SPAWN : public BasePacket {
		virtual ~Packet_SPAWN() {}
		PacketType type() { return SPAWN_PACKET; }

		Point position;
		Vector velocity;
		bool is_grabbing_obstacle;
		bool is_alive;
		uint64_t freeze_time;

		void marshal(PacketWriter& w) {}
		void unmarshal(PacketReader& r) {}
		void dispatch(PacketReceiver* r);
	};
}

#endif
