/*
 * server/Server.cpp
 *
 * Copyright 2009 - Nathan Partlan, Andrew Ayer, Daniel Schneider, and Jeffrey Pfau
 * 
 */

#include "Server.hpp"
#include "ServerNetwork.hpp"
#include "common/LMException.hpp"
#include "common/PacketWriter.hpp"
#include "common/PacketReader.hpp"
#include "common/network.hpp"
#include <string>
#include <iostream>

using namespace std;

// This can't be an enum because we want overloading of operator<< to work OK.
const int	Server::SERVER_PROTOCOL_VERSION = 1;

Server::Server ()
{
	m_next_player_id = 1;
	m_is_running = false;
	m_gate_times[0] = m_gate_times[1] = 0;
	m_gate_holders[0] = m_gate_holders[1] = 0;
}

void	Server::player_update(int channel, PacketReader& packet)
{
	// Just broadcast this packet to all other players
	rebroadcast_packet(packet, channel);
}

void	Server::player_shot(int channel, PacketReader& packet)
{
	// Just broadcast this packet to all other players
	// But add the time to unfreeze to the end, as per the network spec
	PacketWriter		resent_packet(PLAYER_SHOT_PACKET);
	resent_packet << packet << int(FREEZE_TIME); // TODO: make time to unfreeze customizable
	m_network.broadcast_packet(resent_packet, channel);
	// TODO: REQUIRE ACK
}

void	Server::gun_fired(int channel, PacketReader& packet)
{
	// Just broadcast this packet to all other players
	rebroadcast_packet(packet, channel);
	// TODO: REQUIRE ACK
}


void	Server::join(int channel, PacketReader& packet)
{
	// XXX: Prototype function ONLY!  Needs to change!
	int			client_version;
	string			name;
	char			team;

	packet >> client_version >> name >> team;

	// TODO: check client version.

	if (team != 'A' && team != 'B') {
		// TODO: Assign to team equitably.
		team = 'A';
	}

	uint32_t		player_id = m_next_player_id++;

	m_players[player_id].init(player_id, channel, client_version, name.c_str(), team);

	// Send the welcome packet back to this client.
	PacketWriter		welcome_packet(WELCOME_PACKET);
	welcome_packet << SERVER_PROTOCOL_VERSION << player_id << team;
	m_network.send_packet(channel, welcome_packet);

	// Tell the player what map is currently in use, and how much time is left in the game
	PacketWriter		game_start_packet(GAME_START_PACKET);
	game_start_packet << m_current_map.get_name() << 86400; // TODO: Time left in game
	m_network.send_packet(channel, game_start_packet); // TODO: REQUIRE ACK

	// Broadcast the announce packet back to all players, except for the new one
	PacketWriter		announce_packet(ANNOUNCE_PACKET);
	announce_packet << player_id << name << team;
	m_network.broadcast_packet(announce_packet, channel);

	// Send the new player an announce packet for every player currently in the game
	player_map::const_iterator	it(m_players.begin());
	while (it != m_players.end()) {
		const ServerPlayer&	player((it++)->second);

		PacketWriter	announce_packet(ANNOUNCE_PACKET);
		announce_packet << player.get_id() << player.get_name() << player.get_team();
		m_network.send_packet(channel, announce_packet);
	}
}

void	Server::leave(int channel, PacketReader& packet)
{
	uint32_t	player_id;
	packet >> player_id;

	m_network.unbind(channel);

	if (is_authorized(channel, player_id)) {
		m_players.erase(player_id);

		// Broadcast to the game that this player has left
		PacketWriter	leave_packet(LEAVE_PACKET);
		leave_packet << player_id;
		m_network.broadcast_packet(leave_packet);

		// If this player was holding down a gate, make sure the gate status is cleared:
		if (gate_is_down('A') && get_gate_holder('A') == player_id) {
			set_gate_time('A', 0);
			report_gate_status('A');
			set_gate_holder('A', 0);
		}
		if (gate_is_down('B') && get_gate_holder('B') == player_id) {
			set_gate_time('B', 0);
			report_gate_status('B');
			set_gate_holder('B', 0);
		}

	}
}

void	Server::run(int portno) // XXX: Prototype function ONLY!
{
	if (!m_current_map.load_file("data/maps/test.map")) { // TODO: Make configurable!
		throw LMException("Failed to load test map.");
	}
	if (!m_network.start(portno)) {
		throw LMException("Failed to start server network on port.");
	}

	m_is_running = true;
	process_input();

	while (m_is_running) {
		time_t		now = time(0);

		// See if a gate has fallen
		if (gate_has_fallen('A', now)) {
			game_over('B');
		} else if (gate_has_fallen('B', now)) {
			game_over('A');
		}
		
		// If a gate is being held down, broadcast a status report on it
		if (gate_is_down('A')) {
			report_gate_status('A');
		}
		if (gate_is_down('B')) {
			report_gate_status('B');
		}
		
		while (m_is_running && m_network.receive_packets(*this, server_sleep_time(time(0)))) {
			process_input();
		}

		process_input();
	}
}

// Arguments:
//  - channel: the channel that the packet is coming from
//  - player_id: the player ID which the packet claims to represent
bool	Server::is_authorized(int channel, uint32_t player_id) const {
	// Look up the player ID in the players map
	player_map::const_iterator	it(m_players.find(player_id));
	
	// Make sure that both:
	//  1. The alleged player actually exists, and
	//  2. The player's stored channel matches the channel that the request is coming from
	return it != m_players.end() && it->second.get_channel() == channel;
}

void	Server::rebroadcast_packet(PacketReader& packet, int exclude_channel) {
	PacketWriter		resent_packet(packet.packet_type());
	resent_packet << packet;
	m_network.broadcast_packet(resent_packet, exclude_channel);
}

void	Server::new_game() {
	PacketWriter		packet(GAME_START_PACKET);
	packet << m_current_map.get_name() << 86400; // TODO: Configurable values here!
	m_network.broadcast_packet(packet);
	set_gate_time('A', 0);
	set_gate_time('B', 0);
	// TODO: REQUIRE ACK
}

void	Server::game_over(char winning_team) {
	PacketWriter		packet(GAME_STOP_PACKET);
	packet << winning_team << 0 << 0; // TODO: send scores
	m_network.broadcast_packet(packet);
	set_gate_time('A', 0);
	set_gate_time('B', 0);
	// TODO: REQUIRE ACK
}

void	Server::spawn_players() {
	m_current_map.reset();
	for (player_map::iterator it(m_players.begin()); it != m_players.end(); ++it) {
		ServerPlayer&		player(it->second);
		if (const Point* point = m_current_map.next_spawnpoint(player.get_team())) {
			PacketWriter	update_packet(PLAYER_UPDATE_PACKET);
			update_packet << player.get_id() << point->x << point->y << 0 << 0 << "";
			m_network.send_packet(player.get_channel(), update_packet);
		} else {
			// Oh noes! No place to spawn this player. TODO: do something about it
		}
	}
	// TODO: REQUIRE ACKs for these
}

void	Server::gate_down(int channel, PacketReader& packet) {
	uint32_t		player_id;
	char			team;
	bool			is_down;
	packet >> player_id >> team >> is_down;

	if (!is_authorized(channel, player_id) || !is_valid_team(team)) {
		// Invalid packet
		return;
	}

	if (gate_is_down(team) == is_down) {
		// Gate already in reported state.
		return;
	}

	if (is_down) {
		// Make the gate go down
		set_gate_time(team, time(0) + GATE_HOLD_TIME);
		set_gate_holder(team, player_id);
		report_gate_status(team);

	} else if (player_id == get_gate_holder(team)) {
		// Make the gate go up
		set_gate_time(team, 0);
		report_gate_status(team);
		set_gate_holder(team, 0);
	}

}

long	Server::server_sleep_time(time_t now) const {
	long		sleep_time = INPUT_POLL_FREQUENCY;

	if (gate_is_down('A')) {
		sleep_time = std::min(sleep_time, std::min(long(GATE_UPDATE_FREQUENCY), time_till_gate_falls('A', now) * 1000L));
	}

	if (gate_is_down('B')) {
		sleep_time = std::min(sleep_time, std::min(long(GATE_UPDATE_FREQUENCY), time_till_gate_falls('B', now) * 1000L));
	}

	return sleep_time;
}

void Server::process_input() {
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT) {
			m_is_running = false;
		}
	}
}

void	Server::set_gate_time(char team, time_t time) {
	m_gate_times[team - 'A'] = time;
}

void	Server::set_gate_holder(char team, uint32_t holder) {
	m_gate_holders[team - 'A'] = holder;
}

long	Server::time_till_gate_falls(char team, time_t now) const {
	long	time = get_gate_time(team);
	return now < time ? time - now : 0L;
}

void	Server::report_gate_status(char team) {
	PacketWriter		packet(GAME_START_PACKET);
	packet << get_gate_holder(team) << team;
	if (gate_is_down(team)) {
		packet << (GATE_HOLD_TIME - time_till_gate_falls(team, time(0))) * 100.0 / GATE_HOLD_TIME;
	} else {
		packet << 0;
	}
	m_network.broadcast_packet(packet);
}

