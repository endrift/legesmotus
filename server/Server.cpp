/*
 * server/Server.cpp
 *
 * This file is part of Leges Motus, a networked, 2D shooter set in zero gravity.
 * 
 * Copyright 2009-2011 Andrew Ayer, Nathan Partlan, Jeffrey Pfau
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

#include "Server.hpp"
#include "ServerConfig.hpp"
#include "ServerNetwork.hpp"
#include "Spawnpoint.hpp"
#include "ClassicMode.hpp"
#include "DeathmatchMode.hpp"
#include "ZombieMode.hpp"
#include "common/Exception.hpp"
#include "common/IPAddress.hpp"
#include "common/PacketWriter.hpp"
#include "common/PacketReader.hpp"
#include "common/network.hpp"
#include "common/team.hpp"
#include "common/misc.hpp"
#include "common/PathManager.hpp"
#include "common/timer.hpp"
#include "common/Version.hpp"
#include "common/GameLogic.hpp"
#include "common/Weapon.hpp"
#include <string>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <iostream>
#include <set>
#include <limits>

using namespace LM;
using namespace std;

const char	Server::SERVER_VERSION[] = LM_VERSION;

Server::Server (ServerConfig& config, PathManager& path_manager) : m_config(config), m_path_manager(path_manager), m_network(*this), m_gates(2, GateStatus(*this))
{
	m_next_player_id = 1;
	m_is_running = false;
	m_game_start_time = 0;
	m_players_have_spawned = false;

	m_register_with_metaserver = true;
	m_last_metaserver_contact_time = 0;
	m_metaserver_token = 0;
	m_metaserver_contact_frequency = 60000; // Initially, report every 60 seconds

	m_team_count[0] = m_team_count[1] = 0;
	m_team_score[0] = m_team_score[1] = 0;
	
	m_game_logic = NULL;
}

void	Server::player_update(const IPAddress& address, PacketReader& inbound_packet)
{
	uint32_t		player_id;
	inbound_packet >> player_id;

	if (is_authorized(address, player_id)) {
		ServerPlayer* player = get_player(player_id);

		// Mark the player as seen
		player->seen(m_timeout_queue);

		// Process the update packet
		player->read_update_packet(inbound_packet);
		
		// Re-broadcast the packet to all _other_ players
		PacketWriter	outbound_packet(PLAYER_UPDATE_PACKET);
		player->write_update_packet(outbound_packet);
		m_network.broadcast_packet(outbound_packet);
	}
}

void	Server::player_animation(const IPAddress& address, PacketReader& inbound_packet)
{
	// Just broadcast this packet to all other players
	uint32_t		player_id;
	inbound_packet >> player_id;

	if (is_authorized(address, player_id)) {
		// Re-broadcast the packet to all _other_ players
		PacketWriter	outbound_packet(PLAYER_ANIMATION_PACKET);
		outbound_packet << player_id << inbound_packet;
		broadcast_packet_except(outbound_packet, player_id);
	}
}

void	Server::name_change(const IPAddress& address, PacketReader& packet)
{
	uint32_t		sender_id = 0;
	string			requested_name;

	packet >> sender_id >> requested_name;

	sanitize_player_name(requested_name);

	if (!is_authorized(address, sender_id) || requested_name.empty()) {
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
	m_network.broadcast_reliable_packet(outbound_packet);
}

void	Server::team_change(const IPAddress& address, PacketReader& packet)
{
	// Parse the packet and sanity-check
	uint32_t		sender_id = 0;
	char			new_team = 0;

	packet >> sender_id >> new_team;

	if (!is_authorized(address, sender_id) || !is_valid_team(new_team)) {
		return;
	}

	ServerPlayer*		player = get_player(sender_id);

	if (player->get_team() == new_team) {
		return;
	}

	if (!m_game_mode->is_team_play()) {
		// TODO: use a REJECT packet instead of sending a system message
		send_system_message(*player, "This game mode does not support team play.");
		return;
	}

	// If team autobalancing is enabled, make sure this team change won't upset the balance
	if (m_params.autobalance_teams && get_team_count(new_team) >= get_team_count(player->get_team())) {
		// TODO: use a REJECT packet instead of sending a system message
		send_system_message(*player, "You may not change teams right now because it would cause an imbalance.");
		return;
	}

	// Make sure the player isn't changing teams too soon
	if (m_players_have_spawned && player->get_team_change_time() >= m_game_start_time && get_ticks() - player->get_team_change_time() < m_params.team_change_period) {
		// TODO: use a REJECT packet instead of sending a system message
		send_system_message(*player, "You are changing teams too often.  Please wait a bit and try again.");
		return;
	}

	// Check to make sure there is space on the current map for the new team
	if (!m_current_map.has_capacity(new_team)) {
		send_system_message(*player, "There are no spawn points left the map for this team.");
		return;
	}

	if (m_players_have_spawned) {
		player->set_team_change_time();
	}

	change_team(*player, new_team, true, false);
}

void	Server::change_team(ServerPlayer& player, char new_team, bool respawn_player, bool respawn_immediately) {
	// Move the player to the new team
	release_player_resources(player);
	player.set_team(new_team);
	++m_team_count[new_team - 'A'];

	if (respawn_player && m_players_have_spawned) {
		if (respawn_immediately) {
			spawn_player(player);
		} else {
			// Hide and freeze the player
			send_spawn_packet(player, NULL, false);
			player.reset_join_time();

			if (m_params.late_join_delay != numeric_limits<uint64_t>::max()) {
				if (m_params.late_spawn_frozen) {
					// Spawn this player frozen
					spawn_player(player, m_params.late_join_delay);
				} else {
					// Add the player to the spawn queue
					m_waiting_players.push_back(&player);
				}
			}
		}
	}

	// Notify all players that this player has switched teams:
	PacketWriter		outbound_packet(TEAM_CHANGE_PACKET);
	outbound_packet << player.get_id() << player.get_team();
	m_network.broadcast_reliable_packet(outbound_packet);
}

void	Server::message(const IPAddress& address, PacketReader& packet)
{
	uint32_t		sender_id = 0;
	string			recipient;
	string			message;

	packet >> sender_id >> recipient >> message;

	if (!is_authorized(address, sender_id)) {
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
		broadcast_team_packet(outbound_packet, recipient[0]);
	} else {
		// Specific player
		uint32_t	recipient_id = atol(recipient.c_str());
		if (const ServerPlayer* recipient_player = get_player(recipient_id)) {
			m_network.send_packet(recipient_player->get_address(), outbound_packet);
		}
	}
}

void	Server::send_system_message(const ServerPlayer& recipient_player, const char* message) {
	PacketWriter	outbound_packet(MESSAGE_PACKET);
	outbound_packet << 0L << recipient_player.get_id() << message;
	m_network.send_reliable_packet(recipient_player.get_address(), outbound_packet);
}

void	Server::broadcast_system_message(const char* message) {
	PacketWriter	outbound_packet(MESSAGE_PACKET);
	outbound_packet << 0L << 0L << message;
	m_network.broadcast_reliable_packet(outbound_packet);
}

void	Server::send_map_list(const ServerPlayer& player) {
	list<string>			files;
	scan_directory(files, m_path_manager.data_path("", "maps"));

	send_system_message(player, "Installed maps:");

	for (list<string>::const_iterator file(files.begin()); file != files.end(); ++file) {
		string::size_type	pos = file->rfind(".map");
		if (pos != string::npos) {
			ostringstream	msg;
			msg << ' ';
			msg.write(file->c_str(), pos);
			send_system_message(player, msg.str().c_str());
		}
	}
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
		send_system_message(*player, "/server maps - Display the maps installed on the server");
		if (player->is_op()) {
			send_system_message(*player, "/server balance - Balance the teams [op]");
			send_system_message(*player, "/server shakeup - Randomize the teams [op]");
			send_system_message(*player, "/server reset - Reset the scores [op]");
			send_system_message(*player, "/server map <mapname> - Load the given map [op]");
			send_system_message(*player, "/server newgame - Start new game [op]");
			send_system_message(*player, "/server kick <player-name> - Kick a player [op]");
			send_system_message(*player, "/server shutdown - Shutdown the server [op]");
		}
		send_system_message(*player, "/server help - Display this help");

	} else if (strncmp(command, "auth ", 5) == 0) {
		if (!m_config.has("password")) {
			send_system_message(*player, "No operator password set.");
		} else if (m_config.get<string>("password") == command + 5) {
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

	} else if (strcmp(command, "maps") == 0) {
		send_map_list(*player);

	} else if (strcmp(command, "shakeup") == 0 && player->is_op()) {
		game_over(0);
		shakeup_teams();
		new_game();

	} else if (strcmp(command, "balance") == 0 && player->is_op()) {
		balance_teams();

	} else if (strcmp(command, "reset") == 0 && player->is_op()) {
		m_team_score[0] = m_team_score[1] = 0;
		// TODO: have to update the team score on the client somehow...
		reset_player_scores();
		send_system_message(*player, "Scores reset.");

	} else if (strncmp(command, "map ", 4) == 0 && player->is_op()) {
		const char*	new_map_name = command + 4;
		if (strpbrk(new_map_name, "/\\") == NULL && load_map(new_map_name)) {
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

void	Server::player_hit(const IPAddress& address, PacketReader& inbound_packet)
{
	uint32_t shooter_id;
	int weapon_id;
	uint32_t shot_player_id;
	bool has_effect;

	inbound_packet >> shooter_id >> weapon_id >> shot_player_id >> has_effect;
		
	Packet::PlayerHit hitdata = Packet::PlayerHit();
	hitdata.shooter_id = shooter_id;
	hitdata.weapon_id = weapon_id;
	hitdata.shot_player_id = shot_player_id;
	hitdata.has_effect = has_effect;
	hitdata.extradata = inbound_packet.get_rest();

	if (!is_authorized(address, shooter_id)) {
		return;
	}

	ServerPlayer* shooter = get_player(shooter_id);
	ServerPlayer* shot_player = get_player(shot_player_id);

	if (!shooter || !shot_player) {
		return;
	}

	// Tell the current game mode that this player was shot
	has_effect = m_game_mode->player_shot(*shooter, *shot_player);
	
	if (shot_player == NULL) {
		WARN("Shot hit player that doesn't exist: " << shot_player_id);
		return;
	}
	
	bool already_frozen = shot_player->is_frozen();
	
	m_game_logic->get_weapon(weapon_id)->hit(shot_player, shooter, &hitdata);

	// Inform the victim that he has been hit
	PacketWriter		outbound_packet(PLAYER_HIT_PACKET);
	outbound_packet << shooter_id << weapon_id << shot_player_id << has_effect << hitdata.extradata;
	m_network.send_reliable_packet(shot_player->get_address(), outbound_packet);
	
	// Send a player_died packet if necessary.
	if (shot_player->is_frozen() && !already_frozen) {
		broadcast_player_died(shooter);
	}
}

void	Server::player_died(const IPAddress& address, PacketReader& packet)
{            
	uint32_t killed_player_id;
	uint32_t killer_id;
	int killer_type;
	uint64_t freeze_time;
	packet >> killed_player_id >> killer_id >> freeze_time >> killer_type;

	if (!is_authorized(address, killed_player_id)) {
		return;
	}

	ServerPlayer& killed_player = *get_player(killed_player_id);
	ServerPlayer* killer = (killer_type == 0) ? get_player(killer_id) : NULL;

	// Tell the current game mode that this player died
	freeze_time = m_game_mode->player_died(killer, killed_player);

	// Inform all players that this player died, and include the freeze time
	PacketWriter		outbound_packet(PLAYER_DIED_PACKET);
	outbound_packet << killed_player_id << killer_id << freeze_time << killer_type;
	m_network.broadcast_reliable_packet(outbound_packet);
}

void	Server::weapon_discharged(const IPAddress& address, PacketReader& inbound_packet)
{
	// Just broadcast this packet to all other players
	uint32_t		shooter_id;
	int			weapon_id;
	string			extradata;
	inbound_packet >> shooter_id >> weapon_id >> extradata;

	if (is_authorized(address, shooter_id)) {
		// Re-broadcast the packet to all _other_ players
		PacketWriter	outbound_packet(WEAPON_DISCHARGED_PACKET);
		outbound_packet << shooter_id << weapon_id << extradata;
		
		// Tell the game logic that there was a weapon fired.
		Weapon* weapon = m_game_logic->get_weapon(weapon_id);
		if (weapon != NULL) {
			weapon->was_fired(m_game_logic->get_world(), *m_game_logic->get_player(shooter_id), extradata);
		}
		
		broadcast_packet_except(outbound_packet, shooter_id);
	}
}


void	Server::join(const IPAddress& address, PacketReader& packet) {
	// Kick unresponsive clients
	timeout_players();

	// Parse the join packet
	int			client_proto_version;
	string			requested_name;
	char			team;
	Version			client_compat_version;

	packet >> client_proto_version;

	cerr << "Join request from " << format_ip_address(address) << ": Client protocol version: " << client_proto_version << ", Client gameplay version: " << client_compat_version << endl;

	if (client_proto_version == PROTOCOL_VERSION) {
		packet >> client_compat_version >> requested_name >> team;
	}

	if (client_proto_version != PROTOCOL_VERSION || client_compat_version != COMPAT_VERSION) {
		cerr << "Rejected join for incompatible client version." << endl;
		reject_join(address, "Incompatible version.  Please upgrade your client.");
		return;
	}

	sanitize_player_name(requested_name);

	// Check player's name for validity
	if (requested_name.empty()) {
		cerr << "Rejected join for empty player name." << endl;
		reject_join(address, "Invalid player name.");
		return;
	}

	// See if this address is already connected to the server
	// This could happen if a player disconnects from the server, but the leave packet is dropped, and they immediately
	// try to join again
	if (ServerPlayer* duplicate_player = get_player_by_address(address)) {
		// Kick the old player
		remove_player(*duplicate_player, "Player joined from same address");
	}

	const bool		is_first_player = m_players.empty();

	if (is_first_player) {
		// This is the first player - reset the team scores
		m_team_score[0] = m_team_score[1] = 0;
	}

	if (!m_game_mode->is_team_play()) {
		team = 'A'; // If team play is not in effect (e.g. for racing or death match), put all players on team A (XXX: a bit hackish)
	} else if (m_params.autobalance_teams || !is_valid_team(team)) {
		// Assign to team equitably.
		if (m_team_count[0] < m_team_count[1]) {
			team = 'A';
		} else if (m_team_count[0] > m_team_count[1]) {
			team = 'B';
		} else if (!is_valid_team(team)) {
			// If teams are equal in size, and the player doesn't have a preference,
			// assign the player to whichever team is losing.
			if (m_team_score[0] < m_team_score[1]) {
				team = 'A';
			} else if (m_team_score[0] > m_team_score[1]) {
				team = 'B';
			} else {
				// Teams are 100% equal - in both size and score -> RANDOM assignment
				team = 'A' + rand() % 2;
			}
		}
		// Note that if autobalancing is enabled but the teams are equal in size, the server will honor the player's team request, if there was one.
	}

	// Check to make sure there is space in the game
	if (nbr_players() >= m_params.max_players) {
		cerr << "Rejected join for " << requested_name << ": No space on server" << endl;
		reject_join(address, "No space on server.");
		return;
	}

	// Check to make sure there is space on the current map
	if (!m_current_map.has_capacity(team)) {
		cerr << "Rejected join for " << requested_name << ": No space on map" << endl;
		reject_join(address, "No space on map.");
		return;
	}

	// Register this player with the network
	m_network.register_peer(address, packet.connection_id(), 1, packet.sequence_no() + 1);

	// Get a unique name for the player
	string			name(get_unique_player_name(requested_name.c_str()));

	++m_team_count[team - 'A'];

	uint32_t		player_id = m_next_player_id++;
	ServerPlayer&		new_player = m_players[player_id].init(player_id, address, client_proto_version, name.c_str(), team, m_timeout_queue);

	cerr << requested_name << ": Joined on team " << team << ", with ID " << player_id << endl;

	if (!m_config.has("password") && address.is_localhost()) {
		// If no operator password was set, give players connecting from the localhost operator privileges
		new_player.set_is_op(true);
	}

	if (is_first_player) {
		// This is the first player.  Start a new game.
		new_game();
	} else {
		// Tell the player about the game
		send_new_round_packets(&new_player);

		if (m_players_have_spawned) {
			if (!m_params.late_spawn_frozen && m_params.late_join_delay != numeric_limits<uint64_t>::max()) {
				// Add the player to the spawn queue
				m_waiting_players.push_back(&new_player);
			}

			// Tell the player that the round has already started
			send_round_start_packet(&new_player);
		}
	}
	
	// Send the player the team scores
	report_team_scores(new_player);

	// Announce the new player
	PacketWriter		announce_packet(ANNOUNCE_PACKET);
	announce_packet << new_player.get_id() << new_player.get_name() << new_player.get_team();
	m_network.broadcast_reliable_packet(announce_packet);

	if (m_players_have_spawned && m_params.late_spawn_frozen && m_params.late_join_delay != numeric_limits<uint64_t>::max()) {
		// Spawn this player frozen
		spawn_player(new_player, m_params.late_join_delay);
	}
	
	// Add the player to our Game Logic, if necessary:
	if (m_game_logic != NULL) {
		m_game_logic->add_player(&new_player);
	}
}

void	Server::info(const IPAddress& address, PacketReader& request_packet) {
	int		client_protocol_version;
	uint32_t	scan_id;
	uint64_t	scan_start_time;
	request_packet >> client_protocol_version >> scan_id >> scan_start_time;

	PacketWriter	response_packet(INFO_server_PACKET);
	response_packet << scan_id << scan_start_time << PROTOCOL_VERSION << COMPAT_VERSION << m_current_map.get_name() << m_team_count[0] << m_team_count[1] << m_params.max_players << get_ticks() << gametime_left() << m_server_name << m_server_location;
	m_network.send_packet(address, response_packet);
}

void	Server::player_jumped(const IPAddress& address, PacketReader& packet) {
	uint32_t	player_id;
	float		direction;
	packet >> player_id >> direction;
	
	if (is_authorized(address, player_id)) {
		// Re-broadcast the packet to all _other_ players
		PacketWriter	outbound_packet(PLAYER_JUMPED_PACKET);
		outbound_packet << player_id << direction;
		
		m_game_logic->attempt_jump(player_id, direction);
		
		broadcast_packet_except(outbound_packet, player_id);
	}
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
	m_network.broadcast_reliable_packet(score_packet);
}

// Report the team scores to given player
void	Server::report_team_scores(const ServerPlayer& recipient_player) {
	{
		PacketWriter	score_packet(SCORE_UPDATE_PACKET);
		score_packet << 'A' << m_team_score[0];
		m_network.send_reliable_packet(recipient_player.get_address(), score_packet);
	}
	{
		PacketWriter	score_packet(SCORE_UPDATE_PACKET);
		score_packet << 'B' << m_team_score[1];
		m_network.send_reliable_packet(recipient_player.get_address(), score_packet);
	}
}

void	Server::leave(const IPAddress& address, PacketReader& packet) {
	uint32_t	player_id;
	string		leave_message;
	packet >> player_id >> leave_message;

	if (is_authorized(address, player_id)) {
		remove_player(*get_player(player_id), leave_message.c_str());
	}
}

void	Server::remove_player(ServerPlayer& player, const char* leave_message) {
	cerr << player.get_name() << ": Leaving: " << leave_message << endl;

	const uint32_t	player_id = player.get_id();

	m_timeout_queue.erase(player.get_timeout_queue_position());

	// Remove this player from the game logic
	if (m_game_logic != NULL) {
		m_game_logic->remove_player(player_id);
	}

	// Broadcast to the game that this player has left
	PacketWriter	leave_packet(LEAVE_PACKET);
	leave_packet << player_id << leave_message;
	m_network.broadcast_reliable_packet(leave_packet);

	// Release resources held by the player (gates, spawn points, team count, etc.)
	release_player_resources(player);

	// Unregister the player from the network
	m_network.unregister_peer(player.get_address());

	// Fully remove the player
	m_players.erase(player_id);

	// Rebalance the teams, if autobalance is on
	if (m_params.autobalance_teams) {
		balance_teams();
	}
}

void	Server::release_player_resources(ServerPlayer& player) {
	// If this player was holding down a gate, make sure the gate status is cleared:
	if (get_gate('A').set_engagement(false, player.get_id())) {
		report_gate_status('A', -1, player.get_id());
	}
	if (get_gate('B').set_engagement(false, player.get_id())) {
		report_gate_status('B', -1, player.get_id());
	}

	// Release/Return the player's spawn point
	if (player.has_spawnpoint()) {
		m_current_map.return_spawnpoint(player.get_team(), player.get_spawnpoint());
		player.set_spawnpoint(NULL);
	}

	m_waiting_players.remove(&player);

	--m_team_count[player.get_team() - 'A'];
}

void	Server::start()
{
	if (!load_map(m_config.get<const char*>("map"))) {
		throw Exception("Failed to load map.");
	}

	if (!resolve_hostname(m_listen_address, m_config.get<const char*>("interface"), m_config.get<uint16_t>("portno"))) {
		throw Exception("Failed to resolve the interface address.  Please make sure it's correct.");
	}

	if (!m_network.start(m_listen_address)) {
		throw Exception("Failed to start server network on interface and port.");
	}

	m_server_name = m_config.get<string>("server_name");
	m_server_location = m_config.get<string>("server_location");

	m_register_with_metaserver = m_config.get<bool>("register_server");

	if (m_register_with_metaserver) {
		// TODO: better error messages if meta server address can't be resolved
		if (const char* metaserver_address = getenv("LM_METASERVER")) {
			// Address specified by $LM_METASERVER environment avariable
			if (!resolve_hostname(m_metaserver_address, metaserver_address)) {
				std::cerr << "Unable to resolve metaserver hostname, `" << metaserver_address << "' as specified in the $LM_METASERVER environment variable.  This server will NOT be registered with the meta server." << std::endl;
				m_register_with_metaserver = false;
			}
		} else if (!resolve_hostname(m_metaserver_address, METASERVER_HOSTNAME, METASERVER_PORTNO)) {
			std::cerr << "Unable to resolve metaserver hostname.  This server will NOT be registered with the meta server." << std::endl;
			m_register_with_metaserver = false;
		}
	}
	if (m_register_with_metaserver) {
		register_with_metaserver();
	}
}

void	Server::run()
{
	m_is_running = true;
	uint64_t last_logic_update = get_ticks();
	
	set<uint32_t> frozen_players;
	
	while (m_is_running) {
		timeout_players();
		m_network.resend_acks();

		if (m_register_with_metaserver && get_ticks() - m_last_metaserver_contact_time >= m_metaserver_contact_frequency) {
			register_with_metaserver();
		}

		if (m_players_have_spawned && !m_players.empty()) {
			// Update the status of the gates
			if (get_gate('A').update()) {
				report_gate_status('A', 0, 0);
			}
			if (get_gate('B').update()) {
				report_gate_status('B', 0, 0);
			}

			m_game_mode->check_state();

			if (get_gate('A').is_open()) {
				m_game_mode->gate_open('A');
			} else if (get_gate('B').is_open()) {
				m_game_mode->gate_open('B');
			}

			if (m_params.game_timeout && time_since_spawn() > m_params.game_timeout) {
				m_game_mode->game_timeout();
			}
			
			// Spawn any players who joined after the game started and are now ready to join:
			spawn_waiting_players();

		} else if (waiting_to_spawn()) {
			if (time_until_spawn() == 0) {
				frozen_players.clear();
				start_game();
			}
		}
		
		uint64_t diff = get_ticks() - last_logic_update;
		if (diff > 10) {
			last_logic_update = get_ticks();
			if (m_game_logic != NULL) {
				uint64_t extratime = m_game_logic->steps(diff);
				
				// Keep track of the extra time between updates.
				last_logic_update -= extratime;
				
				// Check for newly-dead players or players engaging gates:
				for (PlayerMap::iterator it(m_players.begin()); it != m_players.end(); ++it) {
					ServerPlayer& player = it->second;
					// Check for gates
					char team = get_other_team(player. get_team());
					bool is_engaged = m_game_logic->is_engaging_gate(player.get_id(), team);
					if (get_gate(team).set_engagement(is_engaged, player.get_id())) {
						report_gate_status(team, is_engaged ? 1 : -1, player.get_id());
					}
				
					// Check for frozen
					if (frozen_players.find(player.get_id()) != frozen_players.end()) {
						if (!player.is_frozen()) {
							frozen_players.erase(player.get_id());
						}
					} else {
						if (player.is_frozen()) {
							frozen_players.insert(player.get_id());
							if (player.get_freeze_source() != NULL) {
								broadcast_player_died(&player);
							}
						}
					}
				}
			}
		}
		
		m_network.receive_packets(server_sleep_time());
	}

	// Kick any players still in the game!
	// XXX: do we still want to send a SHUTDOWN packet?  Maybe SHUTDOWN is not necessary...
	while (!m_players.empty()) {
		remove_player(m_players.begin()->second, "Server shutting down");
	}

	if (m_register_with_metaserver) {
		unregister_with_metaserver();
	}
}

void	Server::stop() {
	m_is_running = false;
}

void	Server::restart() { // TODO
}

// Arguments:
//  - address: the address that the packet is coming from
//  - player_id: the player ID which the packet claims to represent
bool	Server::is_authorized(const IPAddress& address, uint32_t player_id) const {
	// Look up the player ID in the players map
	const ServerPlayer*	player = get_player(player_id);
	
	// Make sure that both:
	//  1. The alleged player actually exists, and
	//  2. The player's stored address matches the address that the request is coming from
	return player != NULL && player->get_address() == address;
}

void	Server::broadcast_team_packet(const PacketWriter& packet, char team) {
	PlayerMap::const_iterator	it(m_players.begin());
	while (it != m_players.end()) {
		if (it->second.get_team() == team) {
			m_network.send_packet(it->second.get_address(), packet);
		}
		++it;
	}
}

void	Server::broadcast_reliable_team_packet(const PacketWriter& packet, char team) {
	PlayerMap::const_iterator	it(m_players.begin());
	while (it != m_players.end()) {
		if (it->second.get_team() == team) {
			m_network.send_reliable_packet(it->second.get_address(), packet);
		}
		++it;
	}
}

void	Server::broadcast_packet_except(const PacketWriter& packet, uint32_t excluded_player_id) {
	const ServerPlayer*		excluded_player = get_player(excluded_player_id);

	m_network.broadcast_packet(packet, excluded_player ? &excluded_player->get_address() : NULL);
}

void	Server::broadcast_reliable_packet_except(const PacketWriter& packet, uint32_t excluded_player_id) {
	const ServerPlayer*		excluded_player = get_player(excluded_player_id);

	m_network.broadcast_reliable_packet(packet, excluded_player ? &excluded_player->get_address() : NULL);
}

void	Server::new_game() {
	m_game_start_time = get_ticks();
	
	delete_game_logic();
	m_game_logic = new GameLogic(&m_current_map);
	
	m_players_have_spawned = false;
	m_gates[0].reset();
	m_gates[1].reset();
	m_waiting_players.clear(); // Waiting players get cleared out and put in general queue...
	m_game_mode->new_game();

	send_new_round_packets();
}

void	Server::game_over(char winning_team) {
	if (is_valid_team(winning_team)) {
		++m_team_score[winning_team - 'A'];
	} else {
		winning_team = '-';
	}

	PacketWriter		packet(ROUND_OVER_PACKET);
	packet << winning_team << m_team_score[0] << m_team_score[1];
	m_network.broadcast_reliable_packet(packet);
	m_gates[0].reset();
	m_gates[1].reset();
	m_game_start_time = 0;
	m_players_have_spawned = false;
	
	string mapname = m_current_map.get_name();
	m_current_map.clear();
	load_map(mapname.c_str());
}

void	Server::start_game() {
	// Only reset player scores when players spawn, so players have an opportunity between rounds to check the leader board for the prior round
	reset_player_scores();

	m_players_have_spawned = true;
	
	m_current_map.reset();

	// XXX why did I have to put this back?
	delete_game_logic();
	m_game_logic = new GameLogic(&m_current_map);

	const std::list<WeaponReader>&	const_weapons(m_weapon_set.get_weapons());
	std::list<WeaponReader> weapons(const_weapons);
	size_t index = 0;

	for (std::list<WeaponReader>::iterator it(weapons.begin()); it != weapons.end(); ++it) {
		Weapon* weapon = Weapon::new_weapon(*it);
		m_game_logic->add_weapon(index, weapon);
		index++;
	}
	
	m_game_logic->update_map();

	for (PlayerMap::iterator it(m_players.begin()); it != m_players.end(); ++it) {
		m_game_logic->add_player(&(it->second));
		spawn_player(it->second);
	}

	// Send the round start packet
	send_round_start_packet();
}

void	Server::spawn_waiting_players() {
	while (!m_waiting_players.empty() && m_waiting_players.front()->is_ready_to_spawn(m_params.late_join_delay)) {
		spawn_player(*m_waiting_players.front());
		m_waiting_players.pop_front();
	}
}

bool	Server::spawn_player(ServerPlayer& player, uint64_t freeze_time) {
	if (const Spawnpoint* point = m_current_map.next_spawnpoint(player.get_team())) {
		player.set_spawnpoint(point);
		player.set_position(point->get_point());
		player.set_velocity(point->get_initial_velocity());
		player.set_is_grabbing_obstacle(point->is_grabbing_obstacle());
		player.set_is_invisible(false);
		if (freeze_time == 0) {
			player.set_is_frozen(false);
			player.set_energy(Player::MAX_ENERGY);
		} else {
			player.set_is_frozen(true, freeze_time);
		}
		send_spawn_packet(player, point, true, freeze_time);
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


void	Server::gate_update(const IPAddress& address, PacketReader& packet) {
	uint32_t		player_id;
	char			team;
	bool			is_engaged;
	packet >> player_id >> team >> is_engaged;

	if (!is_authorized(address, player_id) || !is_valid_team(team)) {
		// Invalid packet
		return;
	}

	if (get_gate(team).set_engagement(is_engaged, player_id)) {
		report_gate_status(team, is_engaged ? 1 : -1, player_id);
	}
}

uint32_t	Server::server_sleep_time() const {
	/*
	 * Intentionally cap the server sleep time at the largest unsigned 32-bit integer,
	 * because this function is returning a uint32_t, since all network timeouts use
	 * uint32_t's.  This is because some OS's (*cough*, Mac OS X, *cough*) have undefined
	 * maximums for timeouts being passed to select().  Keeping everything 32-bit means we
	 * shouldn't exceed this undefined max.  It's OK if the server has to wake up every 49
	 * days.
	 * ("A wait of 49 days should be sufficient to keep it from busywaiting" -- Jeffrey)
	 */
	uint64_t	sleep_time = std::numeric_limits<uint32_t>::max();

	if (m_register_with_metaserver) {
		uint64_t	time_since_contact = get_ticks() - m_last_metaserver_contact_time;
		if (time_since_contact < m_metaserver_contact_frequency) {
			sleep_time = std::min(sleep_time, m_metaserver_contact_frequency - time_since_contact);
		} else {
			sleep_time = 0;
		}
	 }

	if (m_players_have_spawned) {
		// Take into account gate changes, and gate status updates
		sleep_time = std::min(sleep_time, get_gate('A').next_update_time());
		sleep_time = std::min(sleep_time, get_gate('B').next_update_time());
	}

	if (waiting_to_spawn()) {
		sleep_time = std::min(sleep_time, time_until_spawn());
	}

	if (m_network.has_ack_packets()) {
		sleep_time = std::min(sleep_time, m_network.time_until_ack_resend());
	}

	return sleep_time;
}

void	Server::report_gate_status(char team, int change_in_players, uint32_t acting_player_id) {
	const GateStatus&	gate(get_gate(team));
	PacketWriter		packet(GATE_UPDATE_PACKET);
	packet << acting_player_id << team << gate.get_progress() << change_in_players << gate.get_nbr_players() << get_ticks();
	if (change_in_players) {
		// This is a specific event - it should be reliable
		m_network.broadcast_reliable_packet(packet);
	} else {
		// This is a general update - it is sent often, so reliability isn't important
		m_network.broadcast_packet(packet);
	}
}

void Server::delete_game_logic() {
	if (m_game_logic != NULL) {
		m_game_logic->unregister_map();
		for (PlayerMap::iterator players(m_players.begin()); players != m_players.end(); ++players) {
			players->second.set_attach_joint(NULL);
			players->second.set_is_invisible(true);
			m_game_logic->remove_player(players->second.get_id());
		}
		delete m_game_logic;
		m_game_logic = NULL;
	}
}

bool	Server::waiting_to_spawn() const {
	return (!m_players_have_spawned && !m_players.empty()) ||
		(m_players_have_spawned && !m_waiting_players.empty());
}
uint64_t Server::time_until_spawn() const {
	if (!m_players_have_spawned && !m_players.empty()) {
		// Game has not started yet, but players have joined.
		// All players spawn m_params.game_start_delay ms after the first player joined
		uint64_t	time_elapsed = get_ticks() - m_game_start_time;
		if (time_elapsed < m_params.game_start_delay) {
			return m_params.game_start_delay - time_elapsed;
		} else {
			return 0;
		}

	} else if (m_players_have_spawned && !m_waiting_players.empty()) {
		// Game has started, and there are players queued up to join.
		// When does the first one spawn?
		return m_waiting_players.front()->time_until_spawn(m_params.late_join_delay);
	}
	return std::numeric_limits<uint64_t>::max();
}

uint64_t Server::time_since_spawn() const {
	if (m_players_have_spawned) {
		return get_ticks() - m_game_start_time - m_params.game_start_delay;
	}
	return 0;
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

ServerPlayer*		Server::get_player_by_address(const IPAddress& address) {
	for (PlayerMap::iterator it(m_players.begin()); it != m_players.end(); ++it) {
		if (it->second.get_address() == address) {
			return &it->second;
		}
	}
	return NULL;
}

void	Server::reject_join(const IPAddress& addr, const char* why) {
	PacketWriter	packet(REQUEST_DENIED_PACKET);
	packet << int(JOIN_PACKET) << why;
	m_network.send_packet(addr, packet);
}

bool	Server::load_map(const char* map_name) {
	string		map_filename(map_name);
	map_filename += ".map";

	if (!m_current_map.load_file(m_path_manager.data_path(map_filename.c_str(), "maps"))) {
		return false;
	}

	// 1. Reset the game parameters to their hard-coded internal defaults
	m_params.reset();

	// 2. Set the default game parameters for this map
	m_params.init_from_config(m_current_map.get_options());

	// 3. Set game parameters that are specified in the server-wide config
	m_params.init_from_config(m_config);

	// 4. Initialize the weapon set
	m_weapon_set.load_file(m_params.weapon_set.c_str(), m_path_manager.data_path(m_params.weapon_set.c_str(), "weapons"));

	// 5. Initialize the game mode for this map
	init_game_mode();

	return true;
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

void	Server::send_spawn_packet(ServerPlayer& player, const Spawnpoint* spawnpoint, bool is_alive, uint64_t freeze_time) {
	// Format:
	//  point~velocity~grabbing?~alive?~freeze_time
	PacketWriter	spawn_packet(SPAWN_PACKET);

	if (spawnpoint) {
		spawn_packet << spawnpoint->get_point() << spawnpoint->get_initial_velocity() << spawnpoint->is_grabbing_obstacle();
	} else {
		spawn_packet << Point() << Vector() << false;
	}

	spawn_packet << is_alive;

	if (is_alive) {
		spawn_packet << freeze_time;
	}

	m_network.send_reliable_packet(player.get_address(), spawn_packet);
}

void	Server::register_with_metaserver() {
	m_last_metaserver_contact_time = get_ticks();

	PacketWriter	packet(REGISTER_SERVER_server_PACKET);
	packet << PROTOCOL_VERSION << SERVER_VERSION << m_listen_address;
	m_network.send_packet(m_metaserver_address, packet);
}

void	Server::unregister_with_metaserver() {
	PacketWriter	packet(UNREGISTER_SERVER_PACKET);
	packet << m_listen_address << m_metaserver_token;
	m_network.send_packet(m_metaserver_address, packet);
}

void	Server::register_server_packet(const IPAddress& address, PacketReader& packet) {
	if (m_register_with_metaserver && address == m_metaserver_address) {
		packet >> m_metaserver_token >> m_metaserver_contact_frequency;
	}
}

void	Server::map_info_packet(const IPAddress& address, PacketReader& request_packet) {
	uint32_t	player_id;
	uint32_t	transmission_id;
	request_packet >> player_id >> transmission_id;
	if (!is_authorized(address, player_id)) {
		return;
	}

	PacketWriter	info_packet(MAP_INFO_PACKET);
	info_packet << transmission_id << m_current_map << m_current_map.nbr_objects();
	m_network.send_reliable_packet(address, info_packet);

	const list<MapReader>&	map_objects(m_current_map.get_objects());
	for (list<MapReader>::const_iterator it(map_objects.begin()); it != map_objects.end(); ++it) {
		PacketWriter	object_packet(MAP_OBJECT_PACKET);
		object_packet << transmission_id << *it;
		m_network.send_reliable_packet(address, object_packet);
	}
}

template<class T> void Server::broadcast_param(const ServerPlayer* player, const char* param_name, const T& param_value) {
	PacketWriter	packet(GAME_PARAM_PACKET);
	packet << param_name << param_value;
	
	// Get the string version, for passing to the GameLogic. Ugly, but it works.
	stringstream out;
	out << param_value;
	string string_value = out.str();
	
	if (player) {
		m_network.send_reliable_packet(player->get_address(), packet);
	} else {
		m_network.broadcast_reliable_packet(packet);
	}
	
	if (m_game_logic == NULL) {
		return;
	}

	m_game_logic->set_param(param_name, string_value);
}

void	Server::broadcast_params(const ServerPlayer* player) {
	broadcast_param(player, "radar_mode", m_params.radar_mode);
	broadcast_param(player, "radar_scale", m_params.radar_scale);
	broadcast_param(player, "radar_blip_duration", m_params.radar_blip_duration);
	broadcast_param(player, "recharge_amount", m_params.recharge_amount);
	broadcast_param(player, "recharge_rate", m_params.recharge_rate);
	broadcast_param(player, "recharge_delay", m_params.recharge_delay);
	broadcast_param(player, "recharge_continuously", m_params.recharge_continuously);
	broadcast_param(player, "jump_velocity", m_params.jump_velocity);
	broadcast_param(player, "weapon_switch_delay", m_params.weapon_switch_delay);
}

void	Server::hole_punch_packet(const IPAddress& address, PacketReader& packet) {
	if (!m_register_with_metaserver || address != m_metaserver_address) {
		return;
	}

	IPAddress	client_address;
	uint32_t	scan_id;
	packet >> client_address >> scan_id;

	PacketWriter	client_packet(HOLE_PUNCH_PACKET);
	client_packet << scan_id;

	m_network.send_packet(client_address, client_packet);
}

void	Server::balance_teams() {
	if (!m_game_mode->is_team_play()) {
		// Game mode doesn't have teams
		return;
	}

	if (!get_unbalanced_team()) {
		// Teams are balanced as they are
		return;
	}

	broadcast_system_message("Team auto-balancing in effect...");
	char	unbalanced_team;
	while (is_valid_team(unbalanced_team = get_unbalanced_team())) {
		if (ServerPlayer* victim = get_random_player(unbalanced_team)) {
			change_team(*victim, get_other_team(unbalanced_team), true, true);
		}
	}
}

void	Server::shakeup_teams() {
	if (!m_game_mode->is_team_play()) {
		// Game mode doesn't have teams
		return;
	}

	broadcast_system_message("Random team shakeup in effect...");

	size_t		players_left = nbr_players();
	size_t		blue_allotment = players_left / 2;

	for (PlayerMap::iterator it(m_players.begin()); it != m_players.end(); ++it) {
		char	new_team;
		if (rand() % players_left < blue_allotment) {
			new_team = 'A';
			--blue_allotment;
		} else {
			new_team = 'B';
		}

		if (it->second.get_team() != new_team) {
			change_team(it->second, new_team, true, true);
		}

		--players_left;
	}
}

char	Server::get_unbalanced_team() const {
	if (m_team_count[0] > m_team_count[1] + 1) {
		return 'A';
	} else if (m_team_count[1] > m_team_count[0] + 1) {
		return 'B';
	}
	
	return 0;
}

ServerPlayer*	Server::get_random_player(char team) {
	if (m_team_count[team - 'A'] == 0) {
		return NULL;
	}

	size_t	player_index = rand() % m_team_count[team - 'A'];

	for (PlayerMap::iterator it(m_players.begin()); it != m_players.end(); ++it) {
		if (it->second.get_team() == team) {
			if (player_index-- == 0) {
				return &it->second;
			}
		}
	}
	return NULL; // Should *NOT* get here
}

void	Server::init_game_mode() {
	if (!m_game_mode.get() || m_game_mode->get_type() != m_params.game_mode) {
		switch (m_params.game_mode) {
		case CLASSIC:
			m_game_mode.reset(new ClassicMode(*this));
			break;
		case DEATHMATCH:
			m_game_mode.reset(new DeathmatchMode(*this));
			break;
		case RACE:
			//m_game_mode.reset(new RaceMode(*this));
			break;
		case ZOMBIE:
			m_game_mode.reset(new ZombieMode(*this));
			break;
		}
	}
}

void	Server::change_score(ServerPlayer& player, int score_change) {
	// Change the score
	player.add_score(score_change);

	// And inform all players of the score update
	broadcast_score_update(player);
}

uint64_t	Server::gametime_left() const {
	return m_params.game_timeout ? m_params.game_timeout - time_since_spawn() : std::numeric_limits<uint64_t>::max();
}


void	Server::send_new_round_packets(const ServerPlayer* player) {
	if (player) {
		PacketWriter		welcome_packet(WELCOME_PACKET);
		welcome_packet << PROTOCOL_VERSION << player->get_id() << player->get_name() << player->get_team();
		m_network.send_reliable_packet(player->get_address(), welcome_packet);
	} else {
		PlayerMap::const_iterator	it(m_players.begin());
		while (it != m_players.end()) {
			const ServerPlayer&	nextplayer((it++)->second);
			// Send the welcome packet back to each client.
			PacketWriter		welcome_packet(WELCOME_PACKET);
			welcome_packet << PROTOCOL_VERSION << nextplayer.get_id() << nextplayer.get_name() << nextplayer.get_team();
			m_network.send_reliable_packet(nextplayer.get_address(), welcome_packet);
		}
	}

	PacketWriter	packet(NEW_ROUND_PACKET);
	packet << m_current_map.get_name() << m_current_map.get_revision() << m_current_map.get_width() << m_current_map.get_height();
	if (m_players_have_spawned) {
		// Round already in progress
		packet << 1 << m_params.late_join_delay;
	} else {
		// Round not yet started
		packet << 0 << time_until_spawn();
	}
	if (player) {
		m_network.send_reliable_packet(player->get_address(), packet);
	} else {
		m_network.broadcast_reliable_packet(packet);
	}

	broadcast_params(player);
	broadcast_weapons(player);
	
	// Send the new player an announce packet and score update packet for every player currently in the game
	PlayerMap::const_iterator	it(m_players.begin());
	while (it != m_players.end()) {
		const ServerPlayer&	otherplayer((it++)->second);

		PacketWriter	announce_packet(ANNOUNCE_PACKET);
		announce_packet << otherplayer.get_id() << otherplayer.get_name() << otherplayer.get_team();
		if (player) {
			m_network.send_reliable_packet(player->get_address(), announce_packet);
		} else {
			broadcast_reliable_packet_except(announce_packet, otherplayer.get_id());
		}

		PacketWriter	score_packet(SCORE_UPDATE_PACKET);
		score_packet << otherplayer.get_id() << otherplayer.get_score();
		if (player) {
			m_network.send_reliable_packet(player->get_address(), score_packet);
		} else {
			broadcast_reliable_packet_except(score_packet, otherplayer.get_id());
		}
	}
}

void	Server::send_round_start_packet(const ServerPlayer* player) {
	PacketWriter	packet(ROUND_START_PACKET);
	packet << gametime_left();
	if (player) {
		m_network.send_reliable_packet(player->get_address(), packet);
	} else {
		m_network.broadcast_reliable_packet(packet);
	}
}

void	Server::broadcast_player_died(const ServerPlayer* dead_player, const ServerPlayer* except) {
	PacketWriter	packet(PLAYER_DIED_PACKET);
	
	PhysicsObject* killer = dead_player->get_freeze_source();
	if (killer == NULL) {
		return;
	}
	
	packet << dead_player->get_id();

	if (killer->get_type() == MapObject::PLAYER) {
		packet << (static_cast<Player*>(killer))->get_id();
	} else {
		packet << 0;
	}
	
	packet << dead_player->get_freeze_time();
	
	packet << killer->get_type();
	
	if (except) {
		m_network.send_reliable_packet(except->get_address(), packet);
	} else {
		m_network.broadcast_reliable_packet(packet);
	}
}

void	Server::broadcast_weapons(const ServerPlayer* player) {
	const std::list<WeaponReader>&	weapons(m_weapon_set.get_weapons());
	size_t				index = 0;

	for (std::list<WeaponReader>::const_iterator it(weapons.begin()); it != weapons.end(); ++it) {
		broadcast_weapon_packet(player, index, *it);
		++index;
	}
}

void	Server::broadcast_weapon_packet(const ServerPlayer* player, size_t index, const WeaponReader& data) {
	PacketWriter	packet(WEAPON_INFO_PACKET);
	packet << index;
	packet << data;
	if (player) {
		m_network.send_reliable_packet(player->get_address(), packet);
	} else {
		m_network.broadcast_reliable_packet(packet);
	}
}

void	Server::excessive_packet_drop(const IPAddress& peer) {
	if (ServerPlayer* player = get_player_by_address(peer)) {
		remove_player(*player, "Excessive packet drop");
	}
}

