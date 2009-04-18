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

class PacketReader;

class Server {
private:
	static const int	SERVER_PROTOCOL_VERSION;
	typedef std::map<uint32_t, ServerPlayer> player_map;	// A map from player_id to the player object

	// in milliseconds
	enum {
		INPUT_POLL_FREQUENCY = 5 * 60 * 1000,	// Poll for input from SDL at least every 5 minutes
		GATE_UPDATE_FREQUENCY = 500		// When a gate is down, update players at least every 500 ms
	};

	// in seconds
	enum {
		FREEZE_TIME = 10,			// Players stay frozen for 10 seconds
		GATE_HOLD_TIME = 15			// Gate must be held down for 15 seconds
	};

	bool			m_is_running;
	ServerNetwork		m_network;
	uint32_t		m_next_player_id;
	player_map		m_players;
	ServerMap		m_current_map;

	// The times (relative to Epoch) at which gates will fall, as long as a player keeps holding it down
	// 0 if the gate is not being held down
	time_t			m_gate_times[2];
	// Who's holding the given gate down? (0 if nobody)
	uint32_t		m_gate_holders[2];

	// Return true if gate for given team is being held down, false otherwise
	bool			gate_is_down(char team) const { return m_gate_times[team - 'A'] > 0; }

	// Get/set the time at which the gates will fall:
	time_t			get_gate_time(char team) const { return m_gate_times[team - 'A']; }
	void			set_gate_time(char team, time_t time);

	// Get/set the player who's holding a gate down:
	uint32_t		get_gate_holder(char team) const { return m_gate_holders[team - 'A']; }
	void			set_gate_holder(char team, uint32_t holder);

	// How long from now until the given gate falls?
	long			time_till_gate_falls(char team, time_t now) const;
	// Has the given gate fallen yet?
	bool			gate_has_fallen(char team, time_t now) const { return gate_is_down(team) && m_gate_times[team - 'A'] <= now; }

	// Make sure the given channel is authorized to speak for given player ID
	bool			is_authorized(int channel, uint32_t player_id) const;

	void			rebroadcast_packet(PacketReader& packet, int exclude_channel =-1);

	void			game_over(char winning_team);

	void			new_game();
	void			spawn_players();

	void			report_gate_status(char team);

	static inline bool	is_valid_team(char c) { return c == 'A' || c == 'B'; }

	void			process_input();			// Process and discard all pending SDL input
	long			server_sleep_time(time_t now) const;	// Maximum time the server should block waiting for packets (in ms)


public:
	Server ();

	// Called upon receipt of network packets:
	void		player_update(int channel, PacketReader& packet);
	void		join(int channel, PacketReader& packet);
	void		leave(int channel, PacketReader& packet);
	void		player_shot(int channel, PacketReader& packet);
	void		gun_fired(int channel, PacketReader& packet);
	void		gate_down(int channel, PacketReader& packet);

	// For testing only (interface likely to change):
	void		run(int portno);
};

#endif
