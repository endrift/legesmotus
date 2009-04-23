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
#include <limits>

using namespace std;

// This can't be an enum because we want overloading of operator<< to work OK.
const int	Server::SERVER_PROTOCOL_VERSION = 1;

Server::Server ()
{
	m_next_player_id = 1;
	m_is_running = false;
	m_game_start_time = 0;
	m_players_have_spawned = false;
}

void	Server::player_update(int channel, PacketReader& packet)
{
	// Just broadcast this packet to all other players
	rebroadcast_packet(packet, channel);
}

void	Server::message(int channel, PacketReader& packet)
{
	uint32_t		sender_id = 0;
	string			recipient;
	string			message;

	packet >> sender_id >> recipient >> message;

	if (!is_authorized(channel, sender_id)) {
		return;
	}

	PacketWriter		outbound_packet(MESSAGE_PACKET);
	outbound_packet << sender_id << recipient << message;

	if (recipient.empty()) {
		// To everyone
		m_network.broadcast_packet(outbound_packet);
	} else if (recipient == "A" || recipient == "B") {
		// Specific Team
		char				recipient_team = recipient[0];
		player_map::const_iterator	it(m_players.begin());
		while (it != m_players.end()) {
			if (it->second.get_team() == recipient_team) {
				m_network.send_packet(it->second.get_channel(), outbound_packet);
			}
			++it;
		}
	} else {
		// Specific player
		uint32_t	recipient_id = atol(recipient.c_str());
		if (const ServerPlayer* recipient_player = get_player(recipient_id)) {
			m_network.send_packet(recipient_player->get_channel(), outbound_packet);
		}
	}
}

void	Server::player_shot(int channel, PacketReader& packet)
{
	// Just broadcast this packet to all other players
	// But add the time to unfreeze to the end, as per the network spec
	PacketWriter		resent_packet(PLAYER_SHOT_PACKET);
	resent_packet << packet << int(FREEZE_TIME);
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

	bool			is_first_player = m_players.empty();

	uint32_t		player_id = m_next_player_id++;
	m_players[player_id].init(player_id, channel, client_version, name.c_str(), team);

	// Send the welcome packet back to this client.
	PacketWriter		welcome_packet(WELCOME_PACKET);
	welcome_packet << SERVER_PROTOCOL_VERSION << player_id << team;
	m_network.send_packet(channel, welcome_packet);

	if (is_first_player) {
		// This is the first player.  Start a new game.
		new_game();
	} else {
		// Joining a game with players.
		// Tell the player what map is currently in use, and how much time until the round starts (i.e. players spawn)
		PacketWriter		game_start_packet(GAME_START_PACKET);
		game_start_packet << m_current_map.get_name() << time_until_spawn();
		m_network.send_packet(channel, game_start_packet); // TODO: REQUIRE ACK
	}

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
		if (get_gate('A').reset_player(player_id)) {
			report_gate_status('A');
		}
		if (get_gate('B').reset_player(player_id)) {
			report_gate_status('B');
		}

	}
}

void	Server::run(int portno)
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
		if (m_players_have_spawned) {
			// See if a gate has fallen
			if (get_gate('A').has_fallen()) {
				game_over('B');
			} else if (get_gate('B').has_fallen()) {
				game_over('A');
			}
			
			// If a gate is being lowered, broadcast a status report on it
			if (get_gate('A').is_lowering()) {
				report_gate_status('A');
			}
			if (get_gate('B').is_lowering()) {
				report_gate_status('B');
			}

		} else if (waiting_to_spawn()) {
			if (time_until_spawn() == 0) {
				spawn_players();
			}
		}
		
		while (m_is_running && m_network.receive_packets(*this, server_sleep_time())) {
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
	const ServerPlayer*	player = get_player(player_id);
	
	// Make sure that both:
	//  1. The alleged player actually exists, and
	//  2. The player's stored channel matches the channel that the request is coming from
	return player != NULL && player->get_channel() == channel;
}

void	Server::rebroadcast_packet(PacketReader& packet, int exclude_channel) {
	PacketWriter		resent_packet(packet.packet_type());
	resent_packet << packet;
	m_network.broadcast_packet(resent_packet, exclude_channel);
}

void	Server::new_game() {
	m_game_start_time = SDL_GetTicks();
	m_players_have_spawned = false;
	m_gates[0].reset();
	m_gates[1].reset();

	PacketWriter		packet(GAME_START_PACKET);
	packet << m_current_map.get_name() << time_until_spawn();
	m_network.broadcast_packet(packet);
	// TODO: REQUIRE ACK
}

void	Server::game_over(char winning_team) {
	PacketWriter		packet(GAME_STOP_PACKET);
	packet << winning_team << 0 << 0; // TODO: send scores
	m_network.broadcast_packet(packet);
	m_gates[0].reset();
	m_gates[1].reset();
	m_game_start_time = 0;
	m_players_have_spawned = false;
	// TODO: REQUIRE ACK
}

void	Server::spawn_players() {
	m_players_have_spawned = true;
	/* TEMPORARILY DISABLE SERVER SPAWNING because it's ANNOYING
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
	*/
	// TODO: REQUIRE ACKs for these

	PacketWriter		packet(GAME_START_PACKET);
	packet << m_current_map.get_name() << 0;
	m_network.broadcast_packet(packet);
}

void	Server::gate_lowering(int channel, PacketReader& packet) {
	uint32_t		player_id;
	char			team;
	bool			is_lowering;
	packet >> player_id >> team >> is_lowering;

	if (!is_authorized(channel, player_id) || !is_valid_team(team)) {
		// Invalid packet
		return;
	}

	if (get_gate(team).set(is_lowering, player_id)) {
		report_gate_status(team);
	}
}

uint32_t	Server::server_sleep_time() const {
	uint32_t	sleep_time = INPUT_POLL_FREQUENCY;

	if (m_players_have_spawned) {
		if (get_gate('A').is_lowering() || get_gate('B').is_lowering()) {
			sleep_time = std::min(sleep_time, uint32_t(GATE_UPDATE_FREQUENCY));
		}
		if (get_gate('A').is_lowering()) {
			sleep_time = std::min(sleep_time, get_gate('A').time_remaining());
		}
		if (get_gate('B').is_lowering()) {
			sleep_time = std::min(sleep_time, get_gate('B').time_remaining());
		}
	} else if (waiting_to_spawn()) {
		sleep_time = std::min(sleep_time, time_until_spawn());
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

void	Server::report_gate_status(char team) {
	const GateStatus&	gate(get_gate(team));
	PacketWriter		packet(GATE_LOWERING_PACKET);
	packet << gate.get_player_id() << team << gate.get_progress();
	m_network.broadcast_packet(packet);
}

Server::GateStatus::GateStatus() {
	reset();
}

uint32_t Server::GateStatus::time_elapsed() const {
	return m_is_lowering ? SDL_GetTicks() - m_start_time : 0;
}

uint32_t Server::GateStatus::time_remaining() const {
	if (m_is_lowering) {
		uint32_t	elapsed_time = time_elapsed();
		if (elapsed_time < GATE_LOWER_TIME) {
			return GATE_LOWER_TIME - elapsed_time;
		} else {
			// Gate should have already fallen.
			return 0;
		}
	} else {
		// Gate is not being lowered.
		return numeric_limits<uint32_t>::max();
	}
}

bool Server::GateStatus::has_fallen() const {
	return m_is_lowering && time_elapsed() >= GATE_LOWER_TIME;
}

double	Server::GateStatus::get_progress() const {
	return m_is_lowering ? time_elapsed() / double(GATE_LOWER_TIME) : 0.0;
}

void	Server::GateStatus::reset() {
	m_is_lowering = false;
	m_player_id = 0;
	m_start_time = 0;
}

bool	Server::GateStatus::reset_player(uint32_t player_id) {
	if (m_is_lowering && m_player_id == player_id) {
		reset();
		return true;
	}
	return false;
}

bool	Server::GateStatus::set(bool new_is_lowering, uint32_t new_player_id) {
	if (m_is_lowering != new_is_lowering) {
		// Only continue if the state of the gate is actually changing...

		if (new_is_lowering) {
			// Gate is being lowered!
			m_is_lowering = true;
			m_player_id = new_player_id;
			m_start_time = SDL_GetTicks();

			return true;
		} else if (m_player_id == new_player_id) {
			// Only the same player is allowed to stop lowering the gate.
			m_is_lowering = false;
			m_player_id = 0;
			m_start_time = 0;

			return true;
		}
	}

	// Nothing changed about the gate
	return false;
}

bool	Server::waiting_to_spawn() const {
	return !m_players.empty() && !m_players_have_spawned;
}
uint32_t Server::time_until_spawn() const {
	if (waiting_to_spawn()) {
		uint32_t	time_elapsed = SDL_GetTicks() - m_game_start_time;
		if (time_elapsed < GRACE_PERIOD) {
			return GRACE_PERIOD - time_elapsed;
		}
	}
	return 0;
}

ServerPlayer*		Server::get_player(uint32_t player_id) {
	player_map::iterator it(m_players.find(player_id));
	return it != m_players.end() ? &it->second : NULL;
}

const ServerPlayer*	Server::get_player(uint32_t player_id) const {
	player_map::const_iterator it(m_players.find(player_id));
	return it != m_players.end() ? &it->second : NULL;
}

