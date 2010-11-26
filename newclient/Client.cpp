/*
 * newclient/Client.cpp
 *
 * This file is part of Leges Motus, a networked, 2D shooter set in zero gravity.
 * 
 * Copyright 2009-2010 Andrew Ayer, Nathan Partlan, Jeffrey Pfau
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
#include <iostream>

using namespace LM;
using namespace std;

Client::Client() : m_network(this) {
	m_logic = NULL;
}

Client::~Client() {
	delete m_logic;
}

void Client::step(uint64_t diff) {
	m_network.receive_packets();

	if (m_logic != NULL) {
		m_controller->update(diff, *m_logic);
		
		int changes = m_controller->get_changes();
		get_player(m_player_id)->set_gun_rotation_radians(m_controller->get_aim());
		
		if (changes & Controller::JUMPING) {
			m_logic->attempt_jump(m_player_id, m_controller->get_aim());
		}
		
		m_logic->step();
	
		Packet p;
		generate_player_update(m_player_id, &p);
		m_network.send_packet(&p);
	}
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

void Client::generate_player_update(uint32_t id, Packet* p) {
	p->type = PLAYER_UPDATE_PACKET;
	Player* player = get_player(id);
	if (player == NULL || p == NULL) {
		return;
	}

	player->generate_player_update(&p->player_update);
}

GameLogic* Client::get_game() {
	return m_logic;
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

void Client::player_update(const Packet& p) {
	Player* player = get_player(p.player_update.player_id);
	if (player == NULL) {
		return;
	}
	player->read_player_update(p.player_update);
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

void Client::round_start(const Packet& p) {
	STUB(Client::round_start);
}

void Client::round_over(const Packet& p) {
	STUB(Client::end_round);
}

void Client::welcome(const Packet& p) {
	INFO("Received welcome: " << p.welcome.player_id);
	if (get_player(p.welcome.player_id) == NULL) {
		Player* player = make_player(p.welcome.player_name->c_str(), p.welcome.player_id, p.welcome.team);
		add_player(player);
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

void Client::leave(const Packet& p) {
	if (p.leave.player_id != m_player_id) {
		remove_player(p.leave.player_id, *p.leave.message);
	} else {
		WARN("We've left the game, according to the server.");
	}
}

void Client::name_change(const Packet& p) {
	Player* player = get_player(p.name_change.player_id);
	if (player == NULL) {
		return;
	}
	player->set_name(p.name_change.name->c_str());
}

void Client::team_change(const Packet& p) {
	Player* player = get_player(p.team_change.player_id);
	if (player == NULL) {
		return;
	}
	player->set_team(p.team_change.name);
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
	// TODO implement the rest
}

Player* Client::make_player(const char* name, uint32_t id, char team) {
	return new Player(name, id, team);
}

Map* Client::make_map() {
	return new Map;
}

void Client::set_controller(Controller* controller) {
	m_controller = controller;
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
		join.join.team = 'A';

		m_network.send_reliable_packet(&join);
	}
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
