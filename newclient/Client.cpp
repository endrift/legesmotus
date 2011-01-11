/*
 * newclient/Client.cpp
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

#include "Client.hpp"
#include "Controller.hpp"
#include "common/Map.hpp"
#include "common/Player.hpp"
#include "common/timer.hpp"
#include "common/misc.hpp"
#include "common/team.hpp"
#include "common/Weapon.hpp"
#include <iostream>

using namespace LM;
using namespace std;

Client::Client() : m_network(this) {
	m_logic = NULL;
	m_curr_weapon = -1;
	m_player_id = -1;
	m_engaging_gate = false;
}

Client::~Client() {
	delete m_logic;
}

uint64_t Client::step(uint64_t diff) {
	m_network.receive_packets();

	if (m_logic == NULL) {
		return 0;
	}
		
	Player* player = get_player(m_player_id);
		
	if (player == NULL) {
		return 0;
	}

	// FIXME: the client sees too many steps if diff is not a multiple of PHYSICS_TIMESTEP
	m_controller->update(diff, *m_logic);
	
	int changes = m_controller->get_changes();

	if (!player->is_frozen()) {
		player->set_gun_rotation_radians(m_controller->get_aim());
	}
	
	// Handle jumping
	if (changes & Controller::JUMPING) {
		m_logic->attempt_jump(m_player_id, m_controller->get_aim());
	}
	
	// Handle firing
	if (changes & Controller::FIRE_WEAPON) {
		attempt_firing();
	}
	
	// Handle weapon switching
	if (changes & Controller::CHANGE_WEAPON) {
		INFO("Setting weapon to ID " << m_controller->get_weapon());
		set_curr_weapon(m_controller->get_weapon());
	}
	
	bool already_frozen = player->is_frozen();
	
	// Step the GameLogic.
	diff = m_logic->steps(diff);

	Packet p;
	generate_player_update(m_player_id, &p);
	m_network.send_packet(&p);
	
	// Check if our player was killed by a map hazard this frame:
	if (!already_frozen && player->is_frozen() && player->get_energy() == 0) {
		generate_player_died(m_player_id, 0, false);
	}
	
	// Check the weapon for hitting any players:
	check_player_hits();
	
	// Check gates for any updates/changes.
	update_gates();

	return diff;
}

const char* Client::get_res_directory() const {
	// TODO get from env
	return LM_DATA_DIR;
}

void Client::add_player(Player* player) {
	if (m_logic == NULL) {
		set_map(make_map());
	}
	m_logic->add_player(player);
}

void Client::set_own_player(uint32_t id) {
	m_player_id = id;
	Player* player = m_logic->get_player(m_player_id);
	if (player != NULL) {
		player->set_current_weapon_id(m_curr_weapon);
	}
}

void Client::remove_player(uint32_t id) {
	remove_player(id, string());
}

void Client::remove_player(uint32_t id, const string& reason) {
	m_logic->remove_player(id);
}

Player* Client::get_player(uint32_t id) {
	if (m_logic == NULL) {
		return NULL;
	}
	return m_logic->get_player(id);
}

void Client::update_gates() {
	if (m_logic == NULL) {
		return;
	}
	
	Player* player = m_logic->get_player(m_player_id);
	if (player == NULL) {
		return;
	}
	
	char team = get_other_team(player->get_team());

	if (m_logic->is_engaging_gate(m_player_id, team)) {
		if (!m_engaging_gate) {
			generate_gate_update(m_player_id, team, true);
		}
		m_engaging_gate = true;
	} else {
		if (m_engaging_gate) {
			generate_gate_update(m_player_id, team, false);
		}
		m_engaging_gate = false;
	}
}

void Client::attempt_firing() {
	bool fired_successfully = m_logic->attempt_fire(m_player_id, m_curr_weapon, m_controller->get_aim());
	if (fired_successfully) {
		generate_weapon_fired(m_curr_weapon, m_player_id);
	}
}

void Client::check_player_hits() {
	Weapon* weapon = m_logic->get_weapon(m_curr_weapon);
	Packet p(PLAYER_HIT_PACKET);
	Packet::PlayerHit* player_hit = weapon->generate_next_hit_packet(&p.player_hit, m_logic->get_player(m_player_id));
	while (player_hit != NULL) {
		p.type = PLAYER_HIT_PACKET;
		m_network.send_reliable_packet(&p);
		player_hit = weapon->generate_next_hit_packet(&p.player_hit, m_logic->get_player(m_player_id));
	}
}

void Client::generate_player_update(uint32_t id, Packet* p) {
	p->type = PLAYER_UPDATE_PACKET;
	Player* player = get_player(id);
	if (player == NULL || p == NULL) {
		return;
	}

	player->generate_player_update(&p->player_update);
}

void Client::generate_weapon_fired(uint32_t weapon_id, uint32_t player_id) {
	Packet weapon_discharged(WEAPON_DISCHARGED_PACKET);
	//weapon_discharged.type = WEAPON_DISCHARGED_PACKET;
	weapon_discharged.weapon_discharged.weapon_id = m_curr_weapon;
	weapon_discharged.weapon_discharged.player_id = m_player_id;
	weapon_discharged.weapon_discharged.extradata = "";
	m_network.send_packet(&weapon_discharged);
}

void Client::generate_gate_update(uint32_t player_id, char team, bool holding) {
	Packet gate_update(GATE_UPDATE_PACKET);
	
	gate_update.gate_update.acting_player_id = player_id;
	gate_update.gate_update.team = team;
	
	// TODO: Fix this packet's construction to add the rest of the values, once the server is changed.
	gate_update.gate_update.progress = (holding ? 1:0);
	
	m_network.send_reliable_packet(&gate_update);
}

void Client::generate_player_died(uint32_t killed_player_id, uint32_t killer_id, bool killer_is_player = false) {
	Packet player_died(PLAYER_DIED_PACKET);
	player_died.player_died.killed_player_id = killed_player_id;
	player_died.player_died.killer_id = killer_id;
	player_died.player_died.killer_type = (killer_is_player ? 0 : 1);
	
	m_network.send_reliable_packet(&player_died);
}

GameLogic* Client::get_game() {
	return m_logic;
}

Weapon* Client::get_curr_weapon() {
	return m_logic->get_weapon(m_curr_weapon);
}

void Client::set_map(Map* map) {
	// XXX move this somewhere better
	if (map == NULL) {
		delete m_logic;
		m_logic = NULL;
	} else if (m_logic == NULL) {
		m_logic = new GameLogic(map);
	}
}

void Client::send_quit() {
	Packet leave(LEAVE_PACKET);
	
	leave.leave.player_id = m_player_id;
	leave.leave.message = "Client quit.";
	
	m_network.send_reliable_packet(&leave);
	
	m_network.disconnect();
}

Player* Client::make_player(const char* name, uint32_t id, char team) {
	return new Player(name, id, team);
}

Map* Client::make_map() {
	return new Map;
}

Weapon* Client::make_weapon(WeaponReader& weapon_data) {
	return Weapon::new_weapon(weapon_data);
}

void Client::set_controller(Controller* controller) {
	m_controller = controller;
}

void Client::set_curr_weapon(uint32_t id) {
	m_curr_weapon = id;

	Player* player = m_logic->get_player(m_player_id);
	if (player != NULL) {
		Weapon* weapon = m_logic->get_weapon(id);
		if (weapon != NULL) {
			weapon->select(player);
		} else {
			WARN("Setting current weapon to a non-existent weapon: " << id);
		}
		player->set_current_weapon_id(m_curr_weapon);
	}
}

void Client::set_running(bool running) {
	m_running = running;
}

void Client::connect(const IPAddress& server_address) {
	if (m_network.connect(server_address)) {
		Packet join(JOIN_PACKET);
		join.join.protocol_number = PROTOCOL_VERSION;
		join.join.compat_version = COMPAT_VERSION;
		join.join.name = "Foo";
		join.join.team = 0;

		m_network.send_reliable_packet(&join);
	}
}

void Client::player_update(const Packet& p) {
	Player* player = get_player(p.player_update.player_id);
	if (player == NULL) {
		return;
	}
	player->read_player_update(p.player_update);

	// XXX if Weapon::select ever has side effects, we need to have a different way of updating the other players' weapons
	Weapon* weapon = get_game()->get_weapon(p.player_update.current_weapon_id);
	if (weapon != NULL) {
		weapon->select(player);
	}
}

void Client::weapon_discharged(const Packet& p) {
	// TODO: Make this do something???
}

void Client::player_hit(const Packet& p) {
	Player* hit_player = m_logic->get_player(p.player_hit.shot_player_id);
	if (hit_player == NULL) {
		WARN("Shot hit player that doesn't exist: " << p.player_hit.shot_player_id);
		return;
	}
	
	bool already_frozen = hit_player->is_frozen();
	
	m_logic->get_weapon(p.player_hit.weapon_id)->hit(hit_player, &p.player_hit);
	
	// Send a player_died packet if necessary.
	if (p.player_hit.shot_player_id == m_player_id && hit_player->is_frozen() && !already_frozen) {
		generate_player_died(p.player_hit.shot_player_id, p.player_hit.shooter_id, true);
	}
}

void Client::new_round(const Packet& p) {
	Map* map;
	if (m_logic == NULL) {
		set_map(make_map());
	}
	map = m_logic->get_map();
	// TODO use time_until_start, remove round_started from packet
	// TODO preload and tell revision instead of loading the whole thing
	if (map->load_file((string(get_res_directory()) + "/maps/" + *p.new_round.map_name + ".map").c_str())) {
		// TODO put back during real map loading
		/*if (map->get_revision() != map_revision) {
			// this is really lame
			delete map;
			map = make_map();
			map->set_width(map_width);
			map->set_height(map_height);
			map->set_revision(map_revision);
		}*/
		// XXX move
	} else {
		map->set_width(p.new_round.map_width);
		map->set_height(p.new_round.map_height);
		map->set_revision(p.new_round.map_revision);
	}
	m_logic->update_map();
}

void Client::round_over(const Packet& p) {
	// TODO: We may need to do other things here, like update scores, etc.

	delete m_logic;
	m_logic = NULL;
}

void Client::welcome(const Packet& p) {
	INFO("Received welcome: " << p.welcome.player_id);
	if (get_player(p.welcome.player_id) == NULL) {
		Player* player = make_player(p.welcome.player_name->c_str(), p.welcome.player_id, p.welcome.team);
		add_player(player);
		player->set_is_invisible(true);
	}
	set_own_player(p.welcome.player_id);
}

void Client::announce(const Packet& p) {
	if (get_player(p.announce.player_id) != NULL) {
		return;
	}
	INFO("Received announce for " << p.announce.player_id << ": " << *p.announce.player_name);
	Player* player = make_player(p.announce.player_name->c_str(), p.announce.player_id, p.announce.team);
	add_player(player);
}

void Client::gate_update(const Packet& p) {
	char team = p.gate_update.team;
	float progress = p.gate_update.progress;
	m_logic->update_gate_progress(team, progress);
	
	// TODO: In graphical client, this will need to update many other things, probably (graphics, etc.).
}

void Client::leave(const Packet& p) {
	if (p.leave.player_id != m_player_id) {
		remove_player(p.leave.player_id, *p.leave.message);
	} else {
		WARN("We've left the game, according to the server.");
		// Disconnect our network connection so we don't send a leave packet to the server.
		m_network.disconnect();
		disconnect();
	}
}

void Client::name_change(const Packet& p) {
	Player* player = get_player(p.name_change.player_id);
	if (player == NULL) {
		return;
	}
	name_change(player, *p.name_change.name);
}

void Client::team_change(const Packet& p) {
	Player* player = get_player(p.team_change.player_id);
	if (player == NULL) {
		return;
	}
	team_change(player, p.team_change.name);
}

void Client::weapon_info(const Packet& p) {
	WeaponReader wr(*p.weapon_info.weapon_data);
	DEBUG("Weapon: " << p.weapon_info.index);
	Weapon* weapon = make_weapon(wr);
	
	if (weapon != NULL) {
		DEBUG(weapon->get_name() << ", " << weapon->get_id());
		m_logic->add_weapon(p.weapon_info.index, weapon);
		if (m_curr_weapon == -1) {
			set_curr_weapon(weapon->get_id());
		}
	} else {
		WARN("Failed to create weapon");
	}
}

void Client::round_start(const Packet& p) {
	STUB(Client::round_start);
}

void Client::spawn(const Packet& p) {
	Player* player = get_player(m_player_id);
	if (player == NULL) {
		WARN("We don't exist, so we can't spawn");
		return;
	}
	player->set_position(*p.spawn.position);
	player->set_velocity(*p.spawn.velocity);
	player->set_rotation_degrees(0);
	player->set_is_grabbing_obstacle(p.spawn.is_grabbing_obstacle);
	
	if (p.spawn.freeze_time <= 0) {
		player->set_is_frozen(false);
	} else {
		player->set_is_frozen(true, p.spawn.freeze_time);
	}
	
	player->set_is_invisible(false);
	// TODO implement the rest
}

void Client::name_change(Player* player, const std::string& new_name) {
	player->set_name(new_name.c_str());
}

void Client::team_change(Player* player, char new_team) {
	player->set_team(new_team);
}

bool Client::running() const {
	return m_running;
}

void Client::run() {
	uint64_t last_time = get_ticks();
	while (true) { // TODO need a way to quit
		uint64_t current_time = get_ticks();
		step(current_time - last_time);
		last_time = current_time;
	}
}

void Client::disconnect() {
	if (m_network.is_connected()) {
		send_quit();
	}
	
	set_running(false);

	delete m_logic;
	m_logic = NULL;
}
