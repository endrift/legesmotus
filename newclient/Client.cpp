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
#include "common/Player.hpp"
#include "common/timer.hpp"

using namespace LM;
using namespace std;

Client::~Client() {
	// TODO
}

void Client::step(uint64_t diff) {
	//m_network.receive_packets();
	 
	m_controller->update(diff, *m_logic);
}

const char* Client::get_res_directory() const {
	// TODO get from env
	return LM_DATA_DIR;
}

void Client::add_player(Player* player) {
	m_logic->add_player(player);
}

void Client::set_own_player(uint32_t id) {
	// TODO figure out if GameLogic needs to know this
}

void Client::remove_player(uint32_t id) {
	m_logic->remove_player(id);
}

Player* Client::get_player(uint32_t id) {
	return m_logic->get_player(id);
}

void Client::begin_game() {
	m_logic = new GameLogic;
}

void Client::end_game() {
	delete m_logic;
	m_logic = NULL;
}

void Client::new_round() {
	// TODO
}

void Client::start_round() {
	// TODO
}

void Client::end_round() {
	// TODO
}

void Client::welcome(uint32_t player_id, string player_name, char team) {
	announce(player_id, player_name, team);
	set_own_player(player_id);
}

void Client::announce(uint32_t player_id, string player_name, char team) {
	Player* player = make_player(player_name.c_str(), player_id, team);
	add_player(player);
}

Player* Client::make_player(const char* name, uint32_t id, char team) {
	return new Player(name, id, team);
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
