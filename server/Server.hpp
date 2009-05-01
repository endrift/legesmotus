/*
 * server/Server.hpp
 *
 * Copyright 2009 - Nathan Partlan, Andrew Ayer, Daniel Schneider, and Jeffrey Pfau
 * 
 */

#ifndef LM_SERVER_SERVER_HPP
#define LM_SERVER_SERVER_HPP

#include "ServerNetwork.hpp"
#include "ServerPlayer.hpp"
#include "ServerMap.hpp"
#include <stdint.h>
#include <map>
#include <list>
#include <utility>

class PacketReader;

class Server {
private:
	static const int	SERVER_PROTOCOL_VERSION;	// Defined in Server.cpp
	typedef std::map<uint32_t, ServerPlayer> PlayerMap;	// A std::map from player_id to the player object

	// Internal time constants - should not be set by user
	// in milliseconds
	enum {
		GATE_UPDATE_FREQUENCY = 100		// When a gate is down, update players at least every 100 ms
	};

public:
	// Configurable game time constants - in the future could be set by user
	// in milliseconds
	enum {
		FREEZE_TIME = 10000,			// Players stay frozen for 10 seconds
		GATE_LOWER_TIME = 15000,		// Gate must be lowered for 15 seconds to fall
		PLAYER_TIMEOUT = 10000,			// Kick players who have not updated for 10 seconds

		// Allow 2 seconds for all players to join at beginning of round before spawning them.
		// TODO: increase to 15 seconds during production
		// If players join after this time period, they will be subject to JOIN_DELAY below
		START_DELAY = 2000,
		JOIN_DELAY = 2000			// Spawn player 2 seconds after he joins mid-round (TODO: increase to 15 during production)
	};

private:
	// Keeps track of the info for a gate:
	class GateStatus {
	private:
		bool		m_is_lowering;		// True if the gate is being lowered
		uint32_t	m_player_id;		// The player who is lowering the gate
		uint32_t	m_start_time;		// The time (in SDL ticks) at which the gate started to be lowered

	public:
		GateStatus();

		bool		is_lowering() const { return m_is_lowering; }
		uint32_t	get_player_id() const { return m_player_id; }

		uint32_t	time_elapsed() const;	// How many milliseconds have passed since gate started lowering?
		uint32_t	time_remaining() const;	// How many milliseconds until the gate FALLS?
		bool		has_fallen() const;	// Returns true if the gate has fallen.
		double		get_progress() const;	// Number in range [0,1] to indicate progress of gate (i.e. 0%-100%)

		void		reset();		// Call at beginning of new games to completely reset

		// Reset this gate if the given player is lowering it:
		// Returns true if the gate state changed, false otherwise
		bool		reset_player(uint32_t player_id);

		// Set the gate to a new state
		// Returns true if the gate state changed, false otherwise
		bool		set(bool is_lowering, uint32_t player_id);
	};


	//
	// Game State
	//
	bool			m_is_running;
	ServerNetwork		m_network;
	uint32_t		m_next_player_id;	// Used to allocate next player ID
	PlayerMap		m_players;
	ServerMap		m_current_map;
	GateStatus		m_gates[2];		// [0] = Team A's gate  [1] = Team B's gate
	uint32_t		m_game_start_time;	// Time at which the game started
	bool			m_players_have_spawned;	// True if any players have spawned
	ServerPlayer::Queue	m_waiting_players;	// Players who have joined after start of round and are waiting to be spawned
	ServerPlayer::Queue	m_timeout_queue;	// A list of players in the order in which they will timeout
	int			m_team_count[2];	// [0] = # of players on team A  [1] == # of players on team B
	int			m_team_score[2];	// [0] = team A's score  [1] = team B's score


	//
	// Gate Helpers
	//
	GateStatus&		get_gate(char team) { return m_gates[team - 'A']; }
	const GateStatus&	get_gate(char team) const { return m_gates[team - 'A']; }

	void			report_gate_status(char team);


	//
	// Network Helpers
	//

	// Make sure the given channel is authorized to speak for given player ID
	bool			is_authorized(int channel, uint32_t player_id) const;

	// Relay the received packet to all clients, except the client on specified channel (if not -1)
	void			rebroadcast_packet(PacketReader& packet, int exclude_channel =-1);

	//
	// Game State Helpers
	//

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
	uint32_t		time_until_spawn() const; // Time in ms until players should spawn

	// Returns the player with given player_id, NULL if not found
	ServerPlayer*		get_player(uint32_t player_id);
	const ServerPlayer*	get_player(uint32_t player_id) const;

	// Remove the given player from the game
	void			remove_player(const ServerPlayer& player);

	//
	// Main Loop Helpers
	//

	// Process and discard all pending SDL input
	// Should be called at least once in every INPUT_POLL_FREQUENCY seconds.
	void			process_input();

	// What's the maximum amount of time the server should sleep for between requests? (in milliseconds)
	uint32_t		server_sleep_time() const;

public:
	Server ();

	// Timeout all old players
	void			timeout_players();

	// Called upon receipt of network packets:
	void		player_update(int channel, PacketReader& packet);
	void		join(int channel, PacketReader& packet);
	void		leave(int channel, PacketReader& packet);
	void		player_shot(int channel, PacketReader& packet);
	void		message(int channel, PacketReader& packet);
	void		gun_fired(int channel, PacketReader& packet);
	void		gate_lowering(int channel, PacketReader& packet);
	void		player_animation(int channel, PacketReader& packet);

	// For testing only (interface likely to change):
	void		run(int portno, const char* map_name); // map_name is NAME of map (excluding .map)
};

#endif
