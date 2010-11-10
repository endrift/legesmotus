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
#include "IPAddress.hpp"
#include <string>

namespace LM {
	struct Packet_ACK {
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
	};

	struct Packet_PLAYER_UPDATE {
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
	};

	struct Packet_WEAPON_DISCHARGED {
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
	};

	struct Packet_PLAYER_HIT {
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
	};

	struct Packet_MESSAGE {
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
	};

	struct Packet_NEW_ROUND {
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
	};

	struct Packet_ROUND_OVER {
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
	};

	struct Packet_SCORE_UPDATE {
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
	};

	struct Packet_WELCOME {
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
	};

	struct Packet_ANNOUNCE {
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
	};

	struct Packet_GATE_UPDATE {
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
	};

	struct Packet_JOIN {
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
	};

	// TODO fill in the rest

	struct Packet_INFO_server {
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
	};

	struct Packet_INFO_client {
		int client_proto_version;
		uint32_t scan_id;
		uint64_t scan_start_time;
		Version client_version;

		void marshal(PacketWriter& w) {}
		void unmarshal(PacketReader& r) {}
	};

	struct Packet_LEAVE {
		uint32_t player_id;
		std::string message;

		void marshal(PacketWriter& w) {}
		void unmarshal(PacketReader& r) {}
	};

	struct Packet_PLAYER_ANIMATION {
		uint32_t player_id;
		std::string sprite_list;
		std::string field;
		int value;

		void marshal(PacketWriter& w) {}
		void unmarshal(PacketReader& r) {}
	};

	struct Packet_REQUEST_DENIED {
		int packet_type;
		std::string message;

		void marshal(PacketWriter& w) {}
		void unmarshal(PacketReader& r) {}
	};

	struct Packet_NAME_CHANGE {
		uint32_t player_id;
		std::string name;

		void marshal(PacketWriter& w) {}
		void unmarshal(PacketReader& r) {}
	};

	struct Packet_TEAM_CHANGE {
		uint32_t player_id;
		char name;

		void marshal(PacketWriter& w) {}
		void unmarshal(PacketReader& r) {}
	};

	struct Packet_REGISTER_SERVER_server {
		int server_protocol_version;
		Version server_version;
		IPAddress server_listen_address;

		void marshal(PacketWriter& w) {}
		void unmarshal(PacketReader& r) {}
	};

	struct Packet_REGISTER_SERVER_metaserver {
		uint32_t metaserver_token;
		uint32_t metaserver_contact_frequency;

		void marshal(PacketWriter& w) {}
		void unmarshal(PacketReader& r) {}
	};	

	struct Packet_UNREGISTER_SERVER {
		IPAddress server_listen_address;
		uint32_t metaserver_token;

		void marshal(PacketWriter& w) {}
		void unmarshal(PacketReader& r) {}
	};

	struct Packet_UPGRADE_AVAILABLE {
		std::string latest_version;

		void marshal(PacketWriter& w) {}
		void unmarshal(PacketReader& r) {}
	};

	struct Packet_MAP_INFO {
		uint32_t transmission_id;
		Map map;
		size_t num_expected_objects;

		void marshal(PacketWriter& w) {}
		void unmarshal(PacketReader& r) {}
	};

	struct Packet_MAP_OBJECT {
		uint32_t transmission_id;
		//MapReceiver object;

		void marshal(PacketWriter& w) {}
		void unmarshal(PacketReader& r) {}
	};

	struct Packet_GAME_PARAM {
		std::string param_name;
		//extra data;

		void marshal(PacketWriter& w) {}
		void unmarshal(PacketReader& r) {}
	};

	struct Packet_HOLE_PUNCH {
		IPAddress client_address;
		uint32_t scan_id;

		void marshal(PacketWriter& w) {}
		void unmarshal(PacketReader& r) {}
	};

	struct Packet_PLAYER_DIED {
		uint32_t killed_player_id;
		uint32_t scan_id;

		void marshal(PacketWriter& w) {}
		void unmarshal(PacketReader& r) {}
	};

	struct Packet_WEAPON_INFO {
		size_t index;
		//WeaponReader weapon_data;

		void marshal(PacketWriter& w) {}
		void unmarshal(PacketReader& r) {}
	};

	struct Packet_ROUND_START {
		uint64_t time_left_in_round;

		void marshal(PacketWriter& w) {}
		void unmarshal(PacketReader& r) {}
	};

	struct Packet_SPAWN {
		Point position;
		Vector velocity;
		bool is_grabbing_obstacle;
		bool is_alive;
		uint64_t freeze_time;

		void marshal(PacketWriter& w) {}
		void unmarshal(PacketReader& r) {}
	};
}

#endif
