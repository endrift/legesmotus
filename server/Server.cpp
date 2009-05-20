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
#include "common/misc.hpp"
#include "common/PathManager.hpp"
#include <string>
#include <cstdlib>
#include <cmath>
#include <iostream>
#include <set>
#include <limits>

using namespace std;

// This can't be an enum because we want overloading of operator<< to work OK.
const int	Server::SERVER_PROTOCOL_VERSION = 1;

Server::Server (PathManager& path_manager) : m_path_manager(path_manager), m_ack_manager(*this)
{
	m_next_player_id = 1;
	m_is_running = false;
	m_game_start_time = 0;
	m_players_have_spawned = false;

	m_team_count[0] = m_team_count[1] = 0;
	m_team_score[0] = m_team_score[1] = 0;
}

void	Server::ack(int /*channel*/, PacketReader& ack_packet) {
	uint32_t		player_id;
	uint32_t		packet_type;
	uint32_t		packet_id;
	ack_packet >> player_id >> packet_type >> packet_id;
	m_ack_manager.ack(player_id, packet_id);
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

void	Server::name_change(int channel, PacketReader& packet)
{
	uint32_t		sender_id = 0;
	string			requested_name;

	packet >> sender_id >> requested_name;

	sanitize_player_name(requested_name);

	if (!is_authorized(channel, sender_id) || requested_name.empty()) {
		return;
	}

	ServerPlayer*		player = get_player(sender_id);

	if (player->compare_name(requested_name.c_str())) {
		// Just a capitalization change
		player->set_name(requested_name.c_str());
	} else {
		// A more major change -> we have to get a unique name for it in case there are conflicts
		string		name(get_unique_player_name(requested_name.c_str()));
		player->set_name(name.c_str());
	}

	PacketWriter		outbound_packet(NAME_CHANGE_PACKET);
	outbound_packet << player->get_id() << player->get_name();
	m_network.broadcast_packet(outbound_packet);
}

void	Server::team_change(int channel, PacketReader& packet)
{
	// Parse the packet and sanity-check
	uint32_t		sender_id = 0;
	char			new_team = 0;

	packet >> sender_id >> new_team;

	if (!is_authorized(channel, sender_id) || !is_valid_team(new_team)) {
		return;
	}

	ServerPlayer*		player = get_player(sender_id);

	if (player->get_team() == new_team) {
		return;
	}

	// Check to make sure there is space on the current map for the new team
	if (m_team_count[new_team - 'A'] >= m_current_map.total_capacity(new_team)) {
		send_system_message(*player, "There are no spawn points left the map for this team.");
		return;
	}

	// Move the player to the new team
	release_player_resources(*player);
	player->set_team(new_team);
	++m_team_count[new_team - 'A'];

	if (m_players_have_spawned) {
		// Hide and freeze the player
		send_spawn_packet(*player, Point(0, 0), false);

		// Add them to the waiting to spawn list
		player->reset_join_time();
		m_waiting_players.push_back(player);
	}

	// Notify all players that this player has switched teams:
	PacketWriter		outbound_packet(TEAM_CHANGE_PACKET);
	outbound_packet << player->get_id() << player->get_team();
	m_network.broadcast_packet(outbound_packet);
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
	if (message.substr(0, 8) == "/server ") {
		command_server(sender_id, message.c_str() + 8);
		return;
	}

	PacketWriter		outbound_packet(MESSAGE_PACKET);
	outbound_packet << sender_id << recipient << message;

	if (recipient.empty()) {
		// To everyone
		m_network.broadcast_packet(outbound_packet);
	} else if (is_valid_team(recipient[0])) {
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

void	Server::send_system_message(const ServerPlayer& recipient_player, const char* message) {
	PacketWriter	outbound_packet(MESSAGE_PACKET);
	outbound_packet << 0L << recipient_player.get_id() << message;
	m_network.send_packet(recipient_player.get_channel(), outbound_packet);
}

void	Server::command_server(uint32_t player_id, const char* command) {
	ServerPlayer*		player = get_player(player_id);
	if (player == NULL) {
		return;
	}

	if (strcmp(command, "help") == 0) {
		send_system_message(*player, "/server auth <password> - Authenticate with given password");
		send_system_message(*player, "/server teamscore - Return the score for each team");
		send_system_message(*player, "/server teamcount - Return the number of players on each team");
		if (player->is_op()) {
			send_system_message(*player, "/server reset - Reset the scores [op]");
			send_system_message(*player, "/server map <mapname> - Load the given map [op]");
			send_system_message(*player, "/server newgame - Start new game [op]");
			send_system_message(*player, "/server kick <player-name> - Kick a player [op]");
			send_system_message(*player, "/server shutdown - Shutdown the server [op]");
		}
		send_system_message(*player, "/server help - Display this help");

	} else if (strncmp(command, "auth ", 5) == 0) {
		if (m_password.empty()) {
			send_system_message(*player, "No operator password set.");
		} else if (m_password == command + 5) {
			player->set_is_op(true);
			send_system_message(*player, "You are now operator.");
		} else {
			send_system_message(*player, "Password incorrect.");
		}

	} else if (strcmp(command, "teamscore") == 0) {
		ostringstream	msg;
		msg << "Blue: " << m_team_score[0] << " / Red: " << m_team_score[1];
		send_system_message(*player, msg.str().c_str());

	} else if (strcmp(command, "teamcount") == 0) {
		ostringstream	msg;
		msg << "Blue Players: " << m_team_count[0] << " / Red Players: " << m_team_count[1];
		send_system_message(*player, msg.str().c_str());

	} else if (strcmp(command, "reset") == 0 && player->is_op()) {
		m_team_score[0] = m_team_score[1] = 0;
		// TODO: have to update the team score on the client somehow...
		reset_player_scores();
		send_system_message(*player, "Scores reset.");

	} else if (strncmp(command, "map ", 4) == 0 && player->is_op()) {
		const char*	new_map_name = command + 4;
		if (strchr(new_map_name, '/') == NULL && load_map(new_map_name)) {
			game_over(0);
			new_game();
		} else {
			send_system_message(*player, "Unable to load requested map.");
		}

	} else if (strcmp(command, "newgame") == 0 && player->is_op()) {
		game_over(0);
		new_game();

	} else if (strncmp(command, "kick ", 5) == 0 && player->is_op()) {
		if (ServerPlayer* victim = get_player_by_name(command + 5)) {
			remove_player(*victim, "Kicked by operator");
			send_system_message(*player, "Player kicked.");
		} else {
			send_system_message(*player, "No player by this name.");
		}

	} else if (strcmp(command, "shutdown") == 0 && player->is_op()) {
		send_system_message(*player, "Server going down after this message.");
		m_is_running = false;

	} else {
		send_system_message(*player, "Unknown command or insufficient privileges.  Try '/server help'.");
	}
}

void	Server::player_shot(int /*channel*/, PacketReader& inbound_packet)
{
	uint32_t		shooter_id;
	uint32_t		shot_player_id;
	double			angle;

	inbound_packet >> shooter_id >> shot_player_id >> angle;

	// Inform all players that this player has been shot
	PacketWriter		outbound_packet(PLAYER_SHOT_PACKET);
	outbound_packet << shooter_id << shot_player_id << int(FREEZE_TIME) << angle;
	m_network.broadcast_packet(outbound_packet);
	// TODO: REQUIRE ACK

	if (ServerPlayer* shooter = get_player(shooter_id)) {
		// Add 1 to the shooter's score
		shooter->add_score(1);

		// And inform all players of the score update
		broadcast_score_update(*shooter);
	}
}

void	Server::gun_fired(int channel, PacketReader& packet)
{
	// Just broadcast this packet to all other players
	rebroadcast_packet(packet, channel);
	// TODO: REQUIRE ACK
}


void	Server::join(const IPaddress& address, PacketReader& packet) {
	// Free up channels by kicking dead players
	timeout_players();

	// Parse the join packet
	int			client_version;
	string			requested_name;
	char			team;

	packet >> client_version;

	cerr << "Join request from " << format_ip_address(address) << ": Client version=" << client_version << endl;

	if (client_version != SERVER_PROTOCOL_VERSION) {
		cerr << "Rejected join for incompatible client version." << endl;
		reject_join(address, "Incompatible version.  Please upgrade your client.");
		return;
	}

	packet >> requested_name >> team;

	sanitize_player_name(requested_name);


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

	// Check player's name for validity
	if (requested_name.empty()) {
		cerr << "Rejected join for empty player name." << endl;
		reject_join(address, "Invalid player name.");
		return;
	}

	/* Put back when maps have player limits
	// Check to make sure there is space on the current map
	if (!m_current_map.has_capacity(team)) {
		reject_join(address, "No space on map.");
		return;
	}
	*/
	// Check to make sure there is space on the current map
	if (m_team_count[team - 'A'] >= m_current_map.total_capacity(team)) {
		cerr << "Rejected join for " << requested_name << ": No space on map" << endl;
		reject_join(address, "No space on map.");
		return;
	}

	// Get a unique name for the player
	string			name(get_unique_player_name(requested_name.c_str()));

	// Try to bind player's address
	int			channel = m_network.bind(address);
	if (channel == -1) {
		cerr << "Rejected join for " << requested_name << ": No space on server" << endl;
		reject_join(address, "No space on server.");
		return;
	}

	++m_team_count[team - 'A'];

	bool			is_first_player = m_players.empty();

	uint32_t		player_id = m_next_player_id++;
	ServerPlayer&		new_player = m_players[player_id].init(player_id, channel, client_version, name.c_str(), team, m_timeout_queue);

	cerr << requested_name << ": Joined on team " << team << ", bound to channel " << channel << ", with ID " << player_id << endl;

	if (m_password.empty() && is_localhost(address)) {
		// If no operator password was set, give players connecting from the localhost operator privileges
		new_player.set_is_op(true);
	}

	// Send the welcome packet back to this client.
	PacketWriter		welcome_packet(WELCOME_PACKET);
	welcome_packet << SERVER_PROTOCOL_VERSION << player_id << name << team;
	m_ack_manager.add_packet(player_id, welcome_packet);
	m_network.send_packet(channel, welcome_packet);

	if (is_first_player) {
		// This is the first player.  Start a new game.
		new_game();
	} else if (!m_players_have_spawned) {
		// Joining a game that hasn't started yet.
		// Tell the player what map is currently in use, and how much time until the round starts (i.e. players spawn)
		PacketWriter		game_start_packet(GAME_START_PACKET);
		game_start_packet << m_current_map.get_name() << 0 << time_until_spawn();
		m_ack_manager.add_packet(player_id, game_start_packet);
		m_network.send_packet(channel, game_start_packet);
	} else {
		// Joining a game that has already started
		// Add the player to the join queue
		m_waiting_players.push_back(&new_player);
		// Tell the player what map is currently in use, and how much time until he spawns
		PacketWriter		game_start_packet(GAME_START_PACKET);
		game_start_packet << m_current_map.get_name() << 1 << uint32_t(JOIN_DELAY);
		m_ack_manager.add_packet(player_id, game_start_packet);
		m_network.send_packet(channel, game_start_packet);
	}

	// Send the new player an announce packet and score update packet for every player currently in the game (except for the one just added)
	PlayerMap::const_iterator	it(m_players.begin());
	while (it != m_players.end()) {
		const ServerPlayer&	player((it++)->second);

		if (player.get_id() == player_id) {
			// Skip the player just added
			continue;
		}

		PacketWriter	announce_packet(ANNOUNCE_PACKET);
		announce_packet << player.get_id() << player.get_name() << player.get_team();
		m_ack_manager.add_packet(player_id, announce_packet);
		m_network.send_packet(channel, announce_packet);

		PacketWriter	score_packet(SCORE_UPDATE_PACKET);
		score_packet << player.get_id() << player.get_score();
		m_ack_manager.add_packet(player_id, score_packet);
		m_network.send_packet(channel, score_packet);
	}

	// Send the player the team scores
	report_team_scores(new_player);

	// Announce the new player
	PacketWriter		announce_packet(ANNOUNCE_PACKET);
	announce_packet << player_id << name << team;
	m_ack_manager.add_broadcast_packet(announce_packet);
	m_network.broadcast_packet(announce_packet);

}

void	Server::info(const IPaddress& address, PacketReader& request_packet) {
	PacketWriter	response_packet(INFO_PACKET);
	response_packet << request_packet.packet_id() << SERVER_PROTOCOL_VERSION << m_current_map.get_name() << m_team_count[0] << m_team_count[1];
	m_network.send_packet(address, response_packet);
}

// Reset the scores for all players, broadcasting score updates for each one
void	Server::reset_player_scores() {
	PlayerMap::iterator	it(m_players.begin());
	while (it != m_players.end()) {
		ServerPlayer&	player((it++)->second);
		player.reset_score();
		broadcast_score_update(player);
	}
}

// Broadcast to all players a score update for the give player
void	Server::broadcast_score_update(const ServerPlayer& player) {
	PacketWriter	score_packet(SCORE_UPDATE_PACKET);
	score_packet << player.get_id() << player.get_score();
	m_ack_manager.add_broadcast_packet(score_packet);
	m_network.broadcast_packet(score_packet);
}

// Report the team scores to given player
void	Server::report_team_scores(const ServerPlayer& recipient_player) {
	{
		PacketWriter	score_packet(SCORE_UPDATE_PACKET);
		score_packet << 'A' << m_team_score[0];
		m_ack_manager.add_packet(recipient_player.get_id(), score_packet);
		m_network.send_packet(recipient_player.get_channel(), score_packet);
	}
	{
		PacketWriter	score_packet(SCORE_UPDATE_PACKET);
		score_packet << 'B' << m_team_score[1];
		m_ack_manager.add_packet(recipient_player.get_id(), score_packet);
		m_network.send_packet(recipient_player.get_channel(), score_packet);
	}
}

void	Server::leave(int channel, PacketReader& packet) {
	uint32_t	player_id;
	string		leave_message;
	packet >> player_id >> leave_message;

	if (is_authorized(channel, player_id)) {
		remove_player(*get_player(player_id), leave_message.c_str());
	}
}

void	Server::remove_player(const ServerPlayer& player, const char* leave_message) {
	cerr << player.get_name() << ": Leaving: " << leave_message << endl;

	const uint32_t	player_id = player.get_id();

	m_waiting_players.remove(const_cast<ServerPlayer*>(&player)); // const_cast OK: only being used for comparison inside erase function
	m_timeout_queue.erase(player.get_timeout_queue_position());

	// Broadcast to the game that this player has left
	PacketWriter	leave_packet(LEAVE_PACKET);
	leave_packet << player_id << leave_message;
	m_network.broadcast_packet(leave_packet);

	// Release resources held by the player (gates, spawn points, team count, etc.)
	release_player_resources(player);

	// Unbind the network socket
	m_network.unbind(player.get_channel());

	// Fully remove the player
	m_players.erase(player_id);
}

void	Server::release_player_resources(const ServerPlayer& player) {
	// If this player was holding down a gate, make sure the gate status is cleared:
	if (get_gate('A').set_engagement(false, player.get_id())) {
		report_gate_status('A', -1);
	}
	if (get_gate('B').set_engagement(false, player.get_id())) {
		report_gate_status('B', -1);
	}

	// Release/Return the player's spawn point
	if (player.has_spawnpoint()) {
		m_current_map.return_spawnpoint(player.get_team(), player.get_spawnpoint());
	}

	--m_team_count[player.get_team() - 'A'];
}

void	Server::start(int portno, const char* map_name)
{
	if (!load_map(map_name)) {
		throw LMException("Failed to load map.");
	}
	if (!m_network.start(portno)) {
		throw LMException("Failed to start server network on port.");
	}
}

void	Server::run()
{
	m_is_running = true;
	while (m_is_running) {
		timeout_players();
		m_ack_manager.resend();

		if (m_players_have_spawned) {
			// Update the status of the gates
			get_gate('A').update();
			get_gate('B').update();

			if (get_gate('A').get_status() == GateStatus::OPEN) {
				// A's gate is open => B wins
				game_over('B');
				new_game();
			} else if (get_gate('B').get_status() == GateStatus::OPEN) {
				// B's gate is open => A wins
				game_over('A');
				new_game();
			}
			
			// Spawn any players who joined after the game started and are now ready to join:
			spawn_waiting_players();

			// If a gate is moving, broadcast a status report on it
			if (get_gate('A').is_moving()) {
				report_gate_status('A', 0);
			}
			if (get_gate('B').is_moving()) {
				report_gate_status('B', 0);
			}

		} else if (waiting_to_spawn()) {
			if (time_until_spawn() == 0) {
				start_game();
			}
		}
		
		m_network.receive_packets(*this, server_sleep_time());
	}

	// Kick any players still in the game!
	// XXX: do we still want to send a SHUTDOWN packet?  Maybe SHUTDOWN is not necessary...
	while (!m_players.empty()) {
		remove_player(m_players.begin()->second, "Server shutting down");
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
	packet << m_current_map.get_name() << 0 << uint32_t(START_DELAY);
	m_ack_manager.add_broadcast_packet(packet);
	m_network.broadcast_packet(packet);
}

void	Server::game_over(char winning_team) {
	if (is_valid_team(winning_team)) {
		++m_team_score[winning_team - 'A'];
	} else {
		winning_team = '-';
	}

	PacketWriter		packet(GAME_STOP_PACKET);
	packet << winning_team << m_team_score[0] << m_team_score[1];
	m_ack_manager.add_broadcast_packet(packet);
	m_network.broadcast_packet(packet);
	m_gates[0].reset();
	m_gates[1].reset();
	m_game_start_time = 0;
	m_players_have_spawned = false;
}

void	Server::start_game() {
	// Only reset player scores when players spawn, so players have an opportunity between rounds to check the leader board for the prior round
	reset_player_scores();

	m_players_have_spawned = true;
	m_current_map.reset();

	for (PlayerMap::iterator it(m_players.begin()); it != m_players.end(); ++it) {
		spawn_player(it->second);
	}

	// Send the game start packet
	PacketWriter		packet(GAME_START_PACKET);
	packet << m_current_map.get_name() << 1 << 0;
	m_ack_manager.add_broadcast_packet(packet);
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
		send_spawn_packet(player, *point, true);
		return true;
	} else {
		// Oh noes! No place to spawn this player.
		send_system_message(player, "There are no spawn points on the map for you.  Try switching teams.");
		return false;
	}
}

void	Server::timeout_players() {
	while (!m_timeout_queue.empty() && m_timeout_queue.front()->has_timed_out()) {
		remove_player(*m_timeout_queue.front(), "Timeout");
	}
}


void	Server::gate_update(int channel, PacketReader& packet) {
	uint32_t		player_id;
	char			team;
	bool			is_engaged;
	packet >> player_id >> team >> is_engaged;

	if (!is_authorized(channel, player_id) || !is_valid_team(team)) {
		// Invalid packet
		return;
	}

	if (get_gate(team).set_engagement(is_engaged, player_id)) {
		report_gate_status(team, is_engaged ? 1 : -1);
	}
}

uint32_t	Server::server_sleep_time() const {
	uint32_t	sleep_time = std::numeric_limits<uint32_t>::max();

	if (m_players_have_spawned) {
		// Take into account gate changes, and gate status updates

		if (get_gate('A').is_moving() || get_gate('B').is_moving()) {
			sleep_time = std::min(sleep_time, uint32_t(GATE_UPDATE_FREQUENCY));
		}
		sleep_time = std::min(sleep_time, get_gate('A').time_remaining());
		sleep_time = std::min(sleep_time, get_gate('B').time_remaining());
	}

	if (waiting_to_spawn()) {
		sleep_time = std::min(sleep_time, time_until_spawn());
	}

	if (m_ack_manager.has_packets()) {
		sleep_time = std::min(sleep_time, m_ack_manager.time_until_resend());
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

void	Server::report_gate_status(char team, int change_in_status) {
	const GateStatus&	gate(get_gate(team));
	PacketWriter		packet(GATE_UPDATE_PACKET);
	packet << gate.get_player_id() << team << gate.get_progress() << change_in_status;
	m_network.broadcast_packet(packet);
}

Server::GateStatus::GateStatus() {
	reset();
}

uint32_t Server::GateStatus::time_elapsed() const {
	return is_moving() ? SDL_GetTicks() - m_start_time : 0;
}

uint32_t Server::GateStatus::time_remaining() const {
	if (is_moving()) {
		uint32_t	elapsed_time = time_elapsed();
		if (m_status == OPENING && elapsed_time < get_open_time()) {
			return get_open_time() - elapsed_time;
		} else if (m_status == CLOSING && elapsed_time < GATE_CLOSE_TIME) {
			return GATE_CLOSE_TIME - elapsed_time;
		}

		// Gate has finished moving.
		return 0;
	} else {
		// Gate is not moving.
		return numeric_limits<uint32_t>::max();
	}
}

void Server::GateStatus::update() {
	if (m_status == OPENING && time_elapsed() >= get_open_time()) {
		m_status = OPEN;
		m_start_time = 0;
	} else if (m_status == CLOSING && time_elapsed() >= GATE_CLOSE_TIME) {
		m_status = CLOSED;
		m_start_time = 0;
	}
}

double	Server::GateStatus::get_progress() const {
	switch (m_status) {
	case CLOSED:
		return 0.0;
	case OPEN:
		return 1.0;
	case OPENING:
		return time_elapsed() / double(get_open_time());
	case CLOSING:
		return 1.0 - time_elapsed() / double(GATE_CLOSE_TIME);
	}
	return 0.0;
}

void	Server::GateStatus::reset() {
	m_status = CLOSED;
	m_players.clear();
	m_start_time = 0;
}

void	Server::GateStatus::set_progress(double progress) {
	if (m_status == OPENING) {
		m_start_time = SDL_GetTicks() - uint32_t(progress * get_open_time());
	} else if (m_status == CLOSING) {
		m_start_time = SDL_GetTicks() - uint32_t((1.0 - progress) * GATE_CLOSE_TIME);
	}
}

bool	Server::GateStatus::set_engagement(bool is_now_engaged, uint32_t new_player_id) {
	bool	gate_was_changed = false;

	if (is_now_engaged) {
		// Gate is being engaged...
		double	old_progress = get_progress();
		if (!is_engaged()) {
			// It wasn't already engaged, so start opening it...
			m_status = OPENING;
			gate_was_changed = true;
		}

		m_players.insert(new_player_id);
		set_progress(old_progress);

	} else if (!is_now_engaged && is_engaged() && m_players.count(new_player_id)) {
		// Gate is being disengaged by a player who was previously engaging it...
		double	old_progress = get_progress();
		m_players.erase(new_player_id);

		if (m_players.empty()) {
			// No players left engaging the gate, so start closing it...
			m_status = CLOSING;
			gate_was_changed = true;
		}
		set_progress(old_progress);
	}

	return gate_was_changed;
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
ServerPlayer*		Server::get_player_by_name(const char* name) {
	for (PlayerMap::iterator it(m_players.begin()); it != m_players.end(); ++it) {
		if (it->second.compare_name(name)) {
			return &it->second;
		}
	}
	return NULL;
}
const ServerPlayer*	Server::get_player_by_name(const char* name) const {
	for (PlayerMap::const_iterator it(m_players.begin()); it != m_players.end(); ++it) {
		if (it->second.compare_name(name)) {
			return &it->second;
		}
	}
	return NULL;
}

void	Server::set_password(const char* pw) {
	m_password = pw;
}

void	Server::reject_join(const IPaddress& addr, const char* why) {
	PacketWriter	packet(REQUEST_DENIED_PACKET);
	packet << int(JOIN_PACKET) << why;
	m_network.send_packet(addr, packet);
}

bool	Server::load_map(const char* map_name) {
	string		map_filename(map_name);
	map_filename += ".map";

	return m_current_map.load_file(m_path_manager.data_path(map_filename.c_str(), "maps"));
}

string	Server::get_unique_player_name(const char* requested_name) const {
	string			name(requested_name);
	int			next_suffix = 1;
	while (get_player_by_name(name.c_str()) != NULL) {
		ostringstream	name_to_try;
		name_to_try << requested_name << '-' << next_suffix++;
		name = name_to_try.str();
	}
	return name;
}

void	Server::ServerAckManager::kick_peer(uint32_t player_id) {
	if (ServerPlayer* player = m_server.get_player(player_id)) {
		m_server.remove_player(*player, "Too much packet drop");
	}
}

void	Server::ServerAckManager::resend_packet(uint32_t player_id, const std::string& data) {
	if (ServerPlayer* player = m_server.get_player(player_id)) {
		m_server.m_network.send_packet(player->get_channel(), data);
	}
}

void	Server::ServerAckManager::add_broadcast_packet(const PacketWriter& packet) {
	set<uint32_t>			player_ids;
	PlayerMap::const_iterator	it(m_server.m_players.begin());
	while (it != m_server.m_players.end()) {
		player_ids.insert((it++)->first);
	}
	AckManager::add_broadcast_packet(player_ids, packet);
}

void	Server::send_spawn_packet(const ServerPlayer& player, Point spawnpoint, bool is_alive) {
	PacketWriter	spawn_packet(PLAYER_UPDATE_PACKET);
	spawn_packet << player.get_id() << spawnpoint.x << spawnpoint.y << 0 << 0 << 0 << (is_alive ? "" : "IF");
	m_ack_manager.add_packet(player.get_id(), spawn_packet);
	m_network.send_packet(player.get_channel(), spawn_packet);
}

