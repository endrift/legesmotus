/*
 * server/Server.hpp
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

#ifndef LM_SERVER_SERVER_HPP
#define LM_SERVER_SERVER_HPP

#include "ServerNetwork.hpp"
#include "ServerPlayer.hpp"
#include "ServerMap.hpp"
#include "GateStatus.hpp"
#include "GameModeHelper.hpp"
#include "common/AckManager.hpp"
#include "common/GameParameters.hpp"
#include "common/team.hpp"
#include <stdint.h>
#include <math.h>
#include <map>
#include <set>
#include <list>
#include <utility>
#include <vector>
#include <memory>

namespace LM {
	class PacketReader;
	class PathManager;
	class IPAddress;
	class ServerConfig;
	
	class Server {
	public:
		static const int	SERVER_PROTOCOL_VERSION;	// Defined in Server.cpp
		static const char	SERVER_VERSION[];		// Defined in Server.cpp
	
		// Internal time constants - should not be set by user
		// in milliseconds
		enum {
			GATE_UPDATE_FREQUENCY = 100,		// When a gate is down, update players at least once every 100 ms
			PLAYER_TIMEOUT = 10000			// Kick players who have not updated for 10 seconds
		};

	private:
		typedef std::map<uint32_t, ServerPlayer> PlayerMap;	// A std::map from player_id to the player object
	
		class ServerAckManager : public AckManager {
			Server&		m_server;
		public:
			explicit ServerAckManager(Server& server) : m_server(server) { }
			virtual void	kick_peer(uint32_t player_id);
			virtual void	resend_packet(uint32_t player_id, const std::string& data);
	
			void		add_broadcast_packet(const PacketWriter& packet);
		};
		friend class ServerAckManager;

		friend class GameModeHelper;
		friend class ClassicMode;
		friend class DeathmatchMode;
		friend class ZombieMode;
	
		//
		// Game State
		//
		ServerConfig&		m_config;
		PathManager&		m_path_manager;
		GameParameters		m_params;
		bool			m_is_running;		// When this is false, run() stops its main loop
		IPAddress		m_listen_address;	// The address the server's listening on
		std::string		m_server_name;		// Informative name to display in server browser
		std::string		m_server_location;	// Informative location to display in server browser
		ServerNetwork		m_network;
		ServerAckManager	m_ack_manager;
		uint32_t		m_next_player_id;	// Used to allocate next player ID
		PlayerMap		m_players;
		ServerMap		m_current_map;
		std::auto_ptr<GameModeHelper>	m_game_mode;
		std::vector<GateStatus>	m_gates;		// [0] = Team A's gate  [1] = Team B's gate
		uint64_t		m_game_start_time;	// Time at which the game started
		bool			m_players_have_spawned;	// True if any players have spawned
		ServerPlayer::Queue	m_waiting_players;	// Players who have joined after start of round and are waiting to spawn
		ServerPlayer::Queue	m_timeout_queue;	// A list of players in the order in which they will timeout
		int			m_team_count[2];	// [0] = # of players on team A  [1] == # of players on team B
		int			m_team_score[2];	// [0] = team A's score  [1] = team B's score
		bool			m_autobalance_teams;
	
	
		//
		// Meta server stuff
		//
		bool			m_register_with_metaserver;
		IPAddress		m_metaserver_address;
		uint64_t		m_last_metaserver_contact_time;	// Time in ticks of the last update
		uint32_t		m_metaserver_token;
		uint32_t		m_metaserver_contact_frequency;
	
		void			register_with_metaserver();
		void			unregister_with_metaserver();
	
		//
		// Gate Helpers
		//
		GateStatus&		get_gate(char team) { return m_gates[team - 'A']; }
		const GateStatus&	get_gate(char team) const { return m_gates[team - 'A']; }
	
		// Broadcast a gate update packet to all players
		//  change_in_players = {-1,0,1}: the change in the number of players who are engaging the gate
		//  acting_player_id = the player who caused this change in gate status
		void			report_gate_status(char team, int change_in_players, uint32_t acting_player_id);
	
	
		//
		// Network Helpers
		//
	
		// Make sure the given address is authorized to speak for given player ID
		bool			is_authorized(const IPAddress& address, uint32_t player_id) const;
	
		// Broadcast a packet to all connected players
		void			broadcast_packet(const PacketWriter& packet);
	
		// Broadcast a packet to all connected players on the given team
		void			broadcast_team_packet(const PacketWriter& packet, char team);
	
		// Broadcast a packet to all connected players, except to the given player
		void			broadcast_packet_except(const PacketWriter& packet, uint32_t excluded_player_id);
	
		// Relay the received packet to all connected players
		void			rebroadcast_packet(const PacketReader& packet);
	
		// Relay the received packet to all connected players, except for the given player
		void			rebroadcast_packet_except(const PacketReader& packet, uint32_t excluded_player_id);
	
		/* */

		// Send all the relevant game parameters to the client (should be called at the beginning of each new game)
		// If player is NULL, broadcast to all players, otherwise only to specific player
		void			broadcast_params(const ServerPlayer* player =NULL);
		template<class T> void	broadcast_param(const ServerPlayer* player, const char* param_name, const T& param_value);

		// Reset the scores for all players, broadcasting score updates for each one (call at beginning of new game)
		void			reset_player_scores();
	
		// Broadcast to all players a score update for the given player
		void			broadcast_score_update(const ServerPlayer&);
	
		// Report to given player the team scores
		void			report_team_scores(const ServerPlayer& recipient_player);

		// Increase (or decrease if negative) the score for a player
		void			change_score(ServerPlayer& player, int score_change);
	
		// Send a packet to spawn a player at specified point
		// If is_alive is true, the player will be un-frozen and visible.
		// If is_alive is false, the player will be frozen and invisible.
		void			send_spawn_packet(ServerPlayer& player, Point spawnpoint, bool is_alive);
	
		// Server command support
		void			broadcast_system_message(const char* message);
		void			send_system_message(const ServerPlayer& recipient, const char* message);
		void			send_map_list(const ServerPlayer& recipient);
		void			command_server(uint32_t player_id, const char* command);
	
		// Timeout all old players
		void			timeout_players();
	
		// If a player can't join (no space on map, invalid player name), call this to send back a REJECT packet:
		void			reject_join(const IPAddress& addr, const char* why);
	
		//
		// Game State Helpers
		//
		
		bool			load_map(const char* mapname);
	
		// Initialize a new game.  A new game should be initialized when:
		//   1. After the first player joins, OR
		//   2. After the game ends
		//  It will reset the game state, and broadcast a game start packet to all players with the map name
		void			new_game();
	
		// Start the game.  Games start START_DELAY milliseconds after new_game() is called.
		//  It will spawn each player.
		void			start_game();
	
		// Spawn all the waiting players who are ready to be spawned:
		void			spawn_waiting_players();
	
		// Spawn the given player:
		//  Unfreeze and place the player at the next available spawn point on the current map.
		//  Returns true if player spawned, false if there's no space on map.
		bool			spawn_player(ServerPlayer& player);
	
		// Call this when a game ends: broadcasts the game over packet and resets the game state
		void			game_over(char winning_team);
	
		bool			waiting_to_spawn() const; // Return true if we're waiting to spawn players
		uint64_t		time_until_spawn() const; // Time in ms until players should spawn
		uint64_t		time_since_spawn() const; // Time in ms since players spawned in this game

		// For rebalancing teams:
		void			balance_teams();
		void			shakeup_teams();
		char			get_unbalanced_team() const;	// Returns the larger of the teams if teams are unbalanced, returns 0 if teams are not unbalanced

		ServerPlayer*		get_random_player(char team);

		void			change_team(ServerPlayer& player, char new_team, bool respawn_player, bool respawn_immediately);
	
		// Returns the player with given player_id, NULL if not found
		ServerPlayer*		get_player(uint32_t player_id);
		const ServerPlayer*	get_player(uint32_t player_id) const;
		ServerPlayer*		get_player_by_name(const char* name);
		const ServerPlayer*	get_player_by_name(const char* name) const;
	
		// Given a player name, return a name that is unique among all player names.
		// Does so by appending a number until no player has the name.
		std::string		get_unique_player_name(const char* requested_name) const;
	
		// Remove the given player from the game
		void			remove_player(ServerPlayer& player, const char* leave_message);
	
		// When a player is removed or switches teams, call this function to release any resources (gate holds, team counts, spawnpoints) that the player holds:
		void			release_player_resources(ServerPlayer& player);

		// How many players are in the game?
		int			nbr_players() const { return m_team_count[0] + m_team_count[1]; }
		int			get_team_count(char team) const { return is_valid_team(team) ? m_team_count[team - 'A'] : 0; }

		// Game Mode
		void			init_game_mode();

		//
		// Main Loop Helpers
		//
	
		// What's the maximum amount of time the server should sleep for between requests? (in milliseconds)
		uint32_t		server_sleep_time() const;
	
	public:
		Server (ServerConfig& config, PathManager& path_manager);
	
		// Get information about gate times
		uint64_t get_gate_open_time(size_t nbr_players) const {
			return nbr_players > 1 ? uint64_t(round(m_params.gate_open_time * log(2.0) / log(double(nbr_players + 1)))) : m_params.gate_open_time;
		}
		uint64_t get_gate_close_time() const { return m_params.gate_close_time; }
		uint64_t get_gate_stick_time() const { return m_params.gate_stick_time; }
	
		// Called upon receipt of network packets:
		void		ack(const IPAddress& address, PacketReader& packet);
		void		player_update(const IPAddress& address, PacketReader& packet);
		void		join(const IPAddress& address, PacketReader& packet);
		void		info(const IPAddress& address, PacketReader& packet);
		void		leave(const IPAddress& address, PacketReader& packet);
		void		player_shot(const IPAddress& address, PacketReader& packet);
		void		message(const IPAddress& address, PacketReader& packet);
		void		gun_fired(const IPAddress& address, PacketReader& packet);
		void		gate_update(const IPAddress& address, PacketReader& packet);
		void		player_animation(const IPAddress& address, PacketReader& packet);
		void		name_change(const IPAddress& address, PacketReader& packet);
		void		team_change(const IPAddress& address, PacketReader& packet);
		void		register_server_packet(const IPAddress& address, PacketReader& packet);
		void		map_info_packet(const IPAddress& address, PacketReader& packet);
		void		hole_punch_packet(const IPAddress& address, PacketReader& packet);
	
		void		start();
		void		run();
		void		stop();
		void		restart();
	};
}

#endif
