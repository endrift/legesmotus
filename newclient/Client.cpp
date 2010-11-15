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
#include <iostream>

using namespace LM;
using namespace std;

Client::Client() {
	m_logic = NULL;
}

Client::~Client() {
	// TODO
}

void Client::step(uint64_t diff) {
	//m_network.receive_packets();
	 
	m_controller->update(diff, *m_logic);
	
	int changes = m_controller->get_changes();
	
	if (changes & Controller::JUMPING) {
		m_logic->attempt_jump(m_player_id, m_controller->get_aim());
	}
	
	m_logic->step();
}

const char* Client::get_res_directory() const {
	// TODO get from env
	return LM_DATA_DIR;
}

void Client::add_player(Player* player) {
	m_logic->add_player(player);
}

void Client::set_own_player(uint32_t id) {
	m_player_id = id;
}

void Client::remove_player(uint32_t id) {
	m_logic->remove_player(id);
}

Player* Client::get_player(uint32_t id) {
	return m_logic->get_player(id);
}

GameLogic* Client::get_game() {
	return m_logic;
}

void Client::set_map(Map* map) {
	// TODO
}

void Client::begin_game(Map* map) {
	if (map == NULL) {
		map = make_map();
	}
	m_logic = new GameLogic(map);
	set_map(map);
}

void Client::end_game() {
	set_map(NULL);
	delete m_logic;
	m_logic = NULL;
}

void Client::packet_new_round(const Packet_NEW_ROUND& p) {
	Map* map = make_map();
	// TODO use time_until_start, remove round_started from packet
	// TODO preload and tell revision instead of loading the whole thing
	if (map->load_file((string(get_res_directory()) + "/maps/" + p.map_name + ".map").c_str())) {
		// TODO put back during real map loading
		/*if (map->get_revision() != map_revision) {
			// this is really lame
			delete map;
			map = make_map();
			map->set_width(map_width);
			map->set_height(map_height);
			map->set_revision(map_revision);
		}*/
	} else {
		map->set_width(p.map_width);
		map->set_height(p.map_height);
		map->set_revision(p.map_revision);
	}
	set_map(map);
}

void Client::start_round() {
	// TODO
}

void Client::end_round() {
	// TODO
}

void Client::packet_welcome(const Packet_WELCOME& p) {
	Player* player = make_player(p.player_name.c_str(), p.player_id, p.team);
	add_player(player);
	set_own_player(p.player_id);
}

void Client::packet_announce(const Packet_ANNOUNCE& p) {
	Player* player = make_player(p.player_name.c_str(), p.player_id, p.team);
	add_player(player);
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
