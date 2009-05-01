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
#include "common/team.hpp"
#include <string>
#include <stdlib.h>
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

	m_team_count[0] = m_team_count[1] = 0;
	m_team_score[0] = m_team_score[1] = 0;
}

void	Server::player_update(int channel, PacketReader& inbound_packet)
{
	uint32_t		player_id;
	inbound_packet >> player_id;

	if (is_authorized(channel, player_id)) {
		// Mark the player as seen
		get_player(player_id)->seen(m_timeout_queue);

		// Re-broadcast the packet to all _other_ players
		PacketWriter	outbound_packet(PLAYER_UPDATE_PACKET);
		outbound_packet << player_id << inbound_packet;
		m_network.broadcast_packet(outbound_packet, channel);

	}
}

void	Server::player_animation(int channel, PacketReader& packet)
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
		PlayerMap::const_iterator	it(m_players.begin());
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

void	Server::player_shot(int channel, PacketReader& inbound_packet)
{
	uint32_t		shooter_id;
	uint32_t		shot_player_id;

	inbound_packet >> shooter_id >> shot_player_id;

	// Inform all players that this player has been shot
	PacketWriter		outbound_packet(PLAYER_SHOT_PACKET);
	outbound_packet << shooter_id << shot_player_id << int(FREEZE_TIME);
	m_network.broadcast_packet(outbound_packet);
	// TODO: REQUIRE ACK

	if (ServerPlayer* shooter = get_player(shooter_id)) {
		// Add 1 to the shooter's score
		shooter->add_score(1);

		// And inform all players of the score update
		PacketWriter	score_packet(SCORE_UPDATE_PACKET);
		score_packet << shooter->get_id() << shooter->get_score();
		m_network.broadcast_packet(score_packet);
		// TODO for scoring:
		//  - Score updates for every player have to be sent when new players join
		//  - Reset all scores to 0 at end of round (hence requring another blanket score update)
	}
}

void	Server::gun_fired(int channel, PacketReader& packet)
{
	// Just broadcast this packet to all other players
	rebroadcast_packet(packet, channel);
	// TODO: REQUIRE ACK
}


void	Server::join(int channel, PacketReader& packet)
{
	int			client_version;
	string			name;
	char			team;

	packet >> client_version >> name >> team;

	// TODO: check client version.

	if (!is_valid_team(team)) {
		// Assign to team equitably.
		if (m_team_count[0] < m_team_count[1]) {
			team = 'A';
		} else if (m_team_count[0] > m_team_count[1]) {
			team = 'B';
		} else {
			team = 'A' + rand() % 2;
		}
	}
	++m_team_count[team - 'A'];

	bool			is_first_player = m_players.empty();

	uint32_t		player_id = m_next_player_id++;
	m_players[player_id].init(player_id, channel, client_version, name.c_str(), team, m_timeout_queue);

	// Send the welcome packet back to this client.
	PacketWriter		welcome_packet(WELCOME_PACKET);
	welcome_packet << SERVER_PROTOCOL_VERSION << player_id << team;
	m_network.send_packet(channel, welcome_packet);

	if (is_first_player) {
		// This is the first player.  Start a new game.
		new_game();
	} else if (!m_players_have_spawned) {
		// Joining a game that hasn't started yet.
		// Tell the player what map is currently in use, and how much time until the round starts (i.e. players spawn)
		PacketWriter		game_start_packet(GAME_START_PACKET);
		game_start_packet << m_current_map.get_name() << time_until_spawn();
		m_network.send_packet(channel, game_start_packet); // TODO: REQUIRE ACK
	} else {
		// Joining a game that has already started
		// Add the player to the join queue
		m_waiting_players.push_back(&m_players[player_id]);
		// Tell the player what map is currently in use, and how much time until he spawns
		PacketWriter		game_start_packet(GAME_START_PACKET);
		game_start_packet << m_current_map.get_name() << uint32_t(JOIN_DELAY);
		m_network.send_packet(channel, game_start_packet); // TODO: REQUIRE ACK
	}

	// Broadcast the announce packet back to all players, except for the new one
	PacketWriter		announce_packet(ANNOUNCE_PACKET);
	announce_packet << player_id << name << team;
	m_network.broadcast_packet(announce_packet, channel);

	// Send the new player an announce packet for every player currently in the game
	PlayerMap::const_iterator	it(m_players.begin());
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

	if (is_authorized(channel, player_id)) {
		remove_player(*get_player(player_id));
	}
}

void	Server::remove_player(const ServerPlayer& player) {
	const uint32_t	player_id = player.get_id();

	m_waiting_players.remove(const_cast<ServerPlayer*>(&player)); // const_cast OK: only being used for comparison inside erase function
	m_timeout_queue.erase(player.get_timeout_queue_position());

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

	// Release/Return the player's spawn point
	if (player.has_spawnpoint()) {
		m_current_map.return_spawnpoint(player.get_team(), player.get_spawnpoint());
	}

	--m_team_count[player.get_team() - 'A'];

	// Unbind the network socket
	m_network.unbind(player.get_channel());

	// Fully remove the player
	m_players.erase(player_id);
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
	while (m_is_running) {
		timeout_players();

		if (m_players_have_spawned) {
			// See if a gate has fallen
			if (get_gate('A').has_fallen()) {
				game_over('B');
				new_game();
			} else if (get_gate('B').has_fallen()) {
				game_over('A');
				new_game();
			}
			
			// Spawn any players who joined after the game started and are now ready to join:
			spawn_waiting_players();

			// If a gate is being lowered, broadcast a status report on it
			if (get_gate('A').is_lowering()) {
				report_gate_status('A');
			}
			if (get_gate('B').is_lowering()) {
				report_gate_status('B');
			}

		} else if (waiting_to_spawn()) {
			if (time_until_spawn() == 0) {
				start_game();
			}
		}
		
		m_network.receive_packets(*this, server_sleep_time());
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
	m_waiting_players.clear(); // Waiting players get cleared out and put in general queue...

	PacketWriter		packet(GAME_START_PACKET);
	packet << m_current_map.get_name() << time_until_spawn();
	m_network.broadcast_packet(packet);
	// TODO: REQUIRE ACK
}

void	Server::game_over(char winning_team) {
	if (is_valid_team(winning_team)) {
		++m_team_score[winning_team - 'A'];
	}

	PacketWriter		packet(GAME_STOP_PACKET);
	packet << winning_team << m_team_score[0] << m_team_score[1];
	m_network.broadcast_packet(packet);
	m_gates[0].reset();
	m_gates[1].reset();
	m_game_start_time = 0;
	m_players_have_spawned = false;
	// TODO: REQUIRE ACK
}

void	Server::start_game() {
	m_players_have_spawned = true;
	m_current_map.reset();

	for (PlayerMap::iterator it(m_players.begin()); it != m_players.end(); ++it) {
		spawn_player(it->second);

	}

	// Send the game start packet (TODO: require ACK)
	PacketWriter		packet(GAME_START_PACKET);
	packet << m_current_map.get_name() << 0;
	m_network.broadcast_packet(packet);
}

void	Server::spawn_waiting_players() {
	while (!m_waiting_players.empty() && m_waiting_players.front()->is_ready_to_spawn()) {
		spawn_player(*m_waiting_players.front());
		m_waiting_players.pop_front();
	}
}

bool	Server::spawn_player(ServerPlayer& player) {
	if (const Point* point = m_current_map.next_spawnpoint(player.get_team())) {
		player.set_spawnpoint(point);
		PacketWriter	update_packet(PLAYER_UPDATE_PACKET);
		update_packet << player.get_id() << point->x << point->y << 0 << 0 << "";
		m_network.send_packet(player.get_channel(), update_packet);
		return true;
	} else {
		// Oh noes! No place to spawn this player. TODO: do something about it
		return false;
	}
	// TODO: require ACK for spawn player packet (XXX: player update packets usually don't require ACKs)
}

void	Server::timeout_players() {
	while (!m_timeout_queue.empty() && m_timeout_queue.front()->has_timed_out()) {
		remove_player(*m_timeout_queue.front());
	}
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
	uint32_t	sleep_time = std::numeric_limits<uint32_t>::max();

	if (m_players_have_spawned) {
		// Take into account gate changes, and gate status updates

		if (get_gate('A').is_lowering() || get_gate('B').is_lowering()) {
			sleep_time = std::min(sleep_time, uint32_t(GATE_UPDATE_FREQUENCY));
		}
		if (get_gate('A').is_lowering()) {
			sleep_time = std::min(sleep_time, get_gate('A').time_remaining());
		}
		if (get_gate('B').is_lowering()) {
			sleep_time = std::min(sleep_time, get_gate('B').time_remaining());
		}
	}

	if (waiting_to_spawn()) {
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
	return (!m_players_have_spawned && !m_players.empty()) ||
		(m_players_have_spawned && !m_waiting_players.empty());
}
uint32_t Server::time_until_spawn() const {
	if (!m_players_have_spawned && !m_players.empty()) {
		// Game has not started yet, but players have joined.
		// All players spawn START_DELAY milliseconds after the first player joined
		uint32_t	time_elapsed = SDL_GetTicks() - m_game_start_time;
		if (time_elapsed < START_DELAY) {
			return START_DELAY - time_elapsed;
		} else {
			return 0;
		}

	} else if (m_players_have_spawned && !m_waiting_players.empty()) {
		// Game has started, and there are players queued up to join.
		// When does the first one spawn?
		return m_waiting_players.front()->time_until_spawn();
	}
	return std::numeric_limits<uint32_t>::max();
}

ServerPlayer*		Server::get_player(uint32_t player_id) {
	PlayerMap::iterator it(m_players.find(player_id));
	return it != m_players.end() ? &it->second : NULL;
}

const ServerPlayer*	Server::get_player(uint32_t player_id) const {
	PlayerMap::const_iterator it(m_players.find(player_id));
	return it != m_players.end() ? &it->second : NULL;
}

