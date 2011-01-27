// Auto-generated by parse_idl.py
#ifndef LM_AUTOGEN_PACKET_HPP
#define LM_AUTOGEN_PACKET_HPP
#include <string>

#include "TypeWrapper.hpp"
#include "UDPPacket.hpp"
#include "PacketReader.hpp"
#include "PacketHeader.hpp"
#include "PacketWriter.hpp"

#include "Version.hpp"
#include "Point.hpp"
#include "IPAddress.hpp"
#include "WeaponReader.hpp"
#include "Map.hpp"

namespace LM {
	enum PacketEnum {
		ACK_PACKET = 0,
		PLAYER_UPDATE_PACKET = 1,
		WEAPON_DISCHARGED_PACKET = 2,
		PLAYER_HIT_PACKET = 3,
		MESSAGE_PACKET = 4,
		NEW_ROUND_PACKET = 5,
		ROUND_OVER_PACKET = 6,
		SCORE_UPDATE_PACKET = 7,
		WELCOME_PACKET = 8,
		ANNOUNCE_PACKET = 9,
		GATE_UPDATE_PACKET = 10,
		JOIN_PACKET = 11,
		INFO_server_PACKET = 12,
		INFO_client_PACKET = 13,
		LEAVE_PACKET = 14,
		PLAYER_ANIMATION_PACKET = 15,
		REQUEST_DENIED_PACKET = 16,
		NAME_CHANGE_PACKET = 17,
		TEAM_CHANGE_PACKET = 18,
		REGISTER_SERVER_server_PACKET = 27,
		REGISTER_SERVER_metaserver_PACKET = 19,
		UNREGISTER_SERVER_PACKET = 20,
		UPGRADE_AVAILABLE_PACKET = 21,
		MAP_INFO_PACKET = 22,
		MAP_OBJECT_PACKET = 23,
		GAME_PARAM_PACKET = 24,
		HOLE_PUNCH_PACKET = 25,
		PLAYER_DIED_PACKET = 26,
		WEAPON_INFO_PACKET = 28,
		ROUND_START_PACKET = 29,
		SPAWN_PACKET = 30,
		PLAYER_JUMPED_PACKET = 31,
	};

	class PacketReceiver;

	struct Packet {
		Packet() { clear(); }
		Packet(PacketEnum type);
		Packet(const Packet& other);
		~Packet();
		void clear();
		void marshal();
		void unmarshal();
		void dispatch(PacketReceiver* r);
		struct Ack {
			uint32_t packet_type;
			uint64_t sequence_no;
		};

		struct PlayerUpdate {
			uint32_t player_id;
			float x;
			float y;
			float x_vel;
			float y_vel;
			float rotation;
			int energy;
			float gun_rotation;
			uint32_t current_weapon_id;
			TypeWrapper<std::string> flags;
		};

		struct WeaponDischarged {
			uint32_t player_id;
			uint32_t weapon_id;
			TypeWrapper<std::string> extradata;
		};

		struct PlayerHit {
			uint32_t shooter_id;
			uint32_t weapon_id;
			uint32_t shot_player_id;
			bool has_effect;
			TypeWrapper<std::string> extradata;
		};

		struct Message {
			uint32_t sender_id;
			TypeWrapper<std::string> recipient;
			TypeWrapper<std::string> message_text;
		};

		struct NewRound {
			TypeWrapper<std::string> map_name;
			int map_revision;
			int map_width;
			int map_height;
			bool game_started;
			uint64_t time_until_start;
		};

		struct RoundOver {
			char winning_team;
			int team_a_score;
			int team_b_score;
		};

		struct ScoreUpdate {
			TypeWrapper<std::string> subject;
			int score;
		};

		struct Welcome {
			int server_version;
			int player_id;
			TypeWrapper<std::string> player_name;
			char team;
		};

		struct Announce {
			uint32_t player_id;
			TypeWrapper<std::string> player_name;
			char team;
		};

		struct GateUpdate {
			uint32_t acting_player_id;
			char team;
			float progress;
			int change_in_players;
			size_t new_nbr_players;
			uint64_t sequence_no;
		};

		struct Join {
			int protocol_number;
			TypeWrapper<Version> compat_version;
			TypeWrapper<std::string> name;
			char team;
		};

		struct InfoServer {
			uint32_t request_packet_id;
			uint64_t scan_start_time;
			TypeWrapper<IPAddress> server_address;
			int server_protocol_version;
			TypeWrapper<Version> server_compat_version;
			TypeWrapper<std::string> current_map_name;
			int team_count_a;
			int team_count_b;
			int max_players;
			uint64_t uptime;
			uint64_t time_left_in_game;
			TypeWrapper<std::string> server_name;
			TypeWrapper<std::string> server_location;
		};

		struct InfoClient {
			int client_proto_version;
			uint32_t scan_id;
			uint64_t scan_start_time;
			TypeWrapper<Version> client_version;
		};

		struct Leave {
			uint32_t player_id;
			TypeWrapper<std::string> message;
		};

		struct PlayerAnimation {
			uint32_t player_id;
			TypeWrapper<std::string> sprite_list;
			TypeWrapper<std::string> field;
			int value;
		};

		struct RequestDenied {
			int packet_type;
			TypeWrapper<std::string> message;
		};

		struct NameChange {
			uint32_t player_id;
			TypeWrapper<std::string> name;
		};

		struct TeamChange {
			uint32_t player_id;
			char name;
		};

		struct RegisterServerServer {
			int server_protocol_version;
			TypeWrapper<Version> server_version;
			TypeWrapper<IPAddress> server_listen_address;
		};

		struct RegisterServerMetaserver {
			uint32_t metaserver_token;
			uint32_t metaserver_contact_frequency;
		};

		struct UnregisterServer {
			TypeWrapper<IPAddress> server_listen_address;
			uint32_t metaserver_token;
		};

		struct UpgradeAvailable {
			TypeWrapper<std::string> latest_version;
		};

		struct MapInfo {
			uint32_t transmission_id;
			TypeWrapper<Map> map;
			size_t num_expected_objects;
		};

		struct MapObject {
			uint32_t transmission_id;
		};

		struct GameParam {
			TypeWrapper<std::string> param_name;
			TypeWrapper<std::string> param_value;
		};

		struct HolePunch {
			TypeWrapper<IPAddress> client_address;
			uint32_t scan_id;
		};

		struct PlayerDied {
			uint32_t killed_player_id;
			uint32_t killer_id;
			uint64_t freeze_time;
			int killer_type;
		};

		struct WeaponInfo {
			size_t index;
			TypeWrapper<WeaponReader> weapon_data;
		};

		struct RoundStart {
			uint64_t time_left_in_round;
		};

		struct Spawn {
			TypeWrapper<Point> position;
			TypeWrapper<Point> velocity;
			bool is_grabbing_obstacle;
			bool is_alive;
			uint64_t freeze_time;
		};

		struct PlayerJumped {
			uint32_t player_id;
			float direction;
		};

		PacketEnum type;
		UDPPacket raw;
		PacketHeader header;

		union {
			Ack ack;
			PlayerUpdate player_update;
			WeaponDischarged weapon_discharged;
			PlayerHit player_hit;
			Message message;
			NewRound new_round;
			RoundOver round_over;
			ScoreUpdate score_update;
			Welcome welcome;
			Announce announce;
			GateUpdate gate_update;
			Join join;
			InfoServer info_server;
			InfoClient info_client;
			Leave leave;
			PlayerAnimation player_animation;
			RequestDenied request_denied;
			NameChange name_change;
			TeamChange team_change;
			RegisterServerServer register_server_server;
			RegisterServerMetaserver register_server_metaserver;
			UnregisterServer unregister_server;
			UpgradeAvailable upgrade_available;
			MapInfo map_info;
			MapObject map_object;
			GameParam game_param;
			HolePunch hole_punch;
			PlayerDied player_died;
			WeaponInfo weapon_info;
			RoundStart round_start;
			Spawn spawn;
			PlayerJumped player_jumped;
		};
	};

	class PacketReceiver {
	public:
		virtual ~PacketReceiver() { }
		virtual void ack(const Packet& p) { }
		virtual void player_update(const Packet& p) { }
		virtual void weapon_discharged(const Packet& p) { }
		virtual void player_hit(const Packet& p) { }
		virtual void message(const Packet& p) { }
		virtual void new_round(const Packet& p) { }
		virtual void round_over(const Packet& p) { }
		virtual void score_update(const Packet& p) { }
		virtual void welcome(const Packet& p) { }
		virtual void announce(const Packet& p) { }
		virtual void gate_update(const Packet& p) { }
		virtual void join(const Packet& p) { }
		virtual void info_server(const Packet& p) { }
		virtual void info_client(const Packet& p) { }
		virtual void leave(const Packet& p) { }
		virtual void player_animation(const Packet& p) { }
		virtual void request_denied(const Packet& p) { }
		virtual void name_change(const Packet& p) { }
		virtual void team_change(const Packet& p) { }
		virtual void register_server_server(const Packet& p) { }
		virtual void register_server_metaserver(const Packet& p) { }
		virtual void unregister_server(const Packet& p) { }
		virtual void upgrade_available(const Packet& p) { }
		virtual void map_info(const Packet& p) { }
		virtual void map_object(const Packet& p) { }
		virtual void game_param(const Packet& p) { }
		virtual void hole_punch(const Packet& p) { }
		virtual void player_died(const Packet& p) { }
		virtual void weapon_info(const Packet& p) { }
		virtual void round_start(const Packet& p) { }
		virtual void spawn(const Packet& p) { }
		virtual void player_jumped(const Packet& p) { }
	};

}

#endif
