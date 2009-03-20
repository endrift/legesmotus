/*
 * GameController.cpp
 *
 * Copyright 2009 - Nathan Partlan, Andrew Ayer, Daniel Schneider, and Jeffrey Pfau
 * 
 */
 
#include "GameController.hpp"
#include "GameWindow.hpp"
#include "ClientNetwork.hpp"
#include "GraphicalPlayer.hpp"
#include "common/PacketReader.hpp"
#include "common/PacketWriter.hpp"
#include "common/network.hpp"

#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

using namespace std;

GameController::GameController() {
	init(1680, 1050, 24, true);
}

GameController::GameController(int width, int height) {
	init(width, height, 24, true);
}

GameController::~GameController() {
	m_window->destroy_instance();
	
	// TEMPORARY SPRITE CODE
	delete new_sprite;
	delete m_crosshairs;
}

void GameController::init(int width, int height, int depth, bool fullscreen) {
	initialize_key_bindings();
	
	m_screen_width = width;
	m_screen_height = height;
	m_pixel_depth = depth;
	m_fullscreen = fullscreen;
	m_quit_game = false;
	m_window = GameWindow::get_instance(m_screen_width, m_screen_height, m_pixel_depth, m_fullscreen);

	// TEMPORARY SPRITE CODE
	new_sprite = new Sprite("data/sprites/blue_full.png");
	m_crosshairs = new Sprite("data/sprites/crosshairs.png");
	m_window->register_sprite(m_crosshairs);
}

void GameController::run(int lockfps) {
	cerr << "SDL window is: " << m_window->get_width() << " pixels wide and " 
		<< m_window->get_height() << " pixels tall." << endl;
	
	unsigned long startframe = SDL_GetTicks();
	unsigned long lastmoveframe = SDL_GetTicks();
	
	/* 1 second / FPS = milliseconds per frame */
	double delay = 1000.0 / lockfps;
	
	while(m_quit_game == false) {
		process_input();
		
		m_network.receive_packets(*this);
		
		if (m_quit_game == true) {
			break;
		}
		
		move_objects((SDL_GetTicks() - lastmoveframe) / delay); // scale all position changes to keep game speed constant. 
		
		lastmoveframe = SDL_GetTicks();
		
		// Update graphics if frame rate is correct
		unsigned long currframe = SDL_GetTicks();
		if((currframe - startframe) >= delay) {
			// the framerate:
			int framerate = (1000/(currframe - startframe));
			
			m_window->redraw();
			startframe = SDL_GetTicks();
		}
	}
	
	disconnect();
}

void GameController::set_screen_dimensions(int width, int height) {
	m_screen_width = width;
	m_screen_height = height;
}

void GameController::process_input() {
	SDL_Event event;
	
	while(SDL_PollEvent(&event) != 0) {
		switch(event.type) {
			case SDL_KEYDOWN:
				//Check which key using: event.key.keysym.sym == SDLK_<SOMETHING>
				if (event.key.keysym.sym == m_key_bindings.jump) {
					attempt_jump();
				}
				break;

			case SDL_KEYUP:
				break;
				
			case SDL_MOUSEMOTION:
				// Send motion to the GameWindow, telling it where the mouse is.
				// Use: event.motion.xrel, event.motion.yrel (changes in position), event.motion.x, event.motion.y
				m_crosshairs->set_x(event.motion.x);
				m_crosshairs->set_y(event.motion.y);
				break;
				
			case SDL_MOUSEBUTTONDOWN:
				// Firing code, use event.button.button, event.button.x, event.button.y
				break;
				
			case SDL_MOUSEBUTTONUP:
				break;
				
			default:
				break;
		}
	}
	
	parse_key_input();
}

void GameController::initialize_key_bindings() {
	// -1 = unused
	m_key_bindings.quit = SDLK_ESCAPE;
	m_key_bindings.jump = SDLK_SPACE;
	m_key_bindings.show_overlay = -1;
	m_key_bindings.show_menu = -1;
	m_key_bindings.open_chat = -1;
}

void GameController::parse_key_input() {
	// For keys that can be held down:
   	m_keys = SDL_GetKeyState(NULL);
	if (m_keys[m_key_bindings.quit]) {
		cerr << "Quit key pressed - quitting." << endl;
		m_quit_game = true;
	} else if (m_keys[m_key_bindings.show_overlay]) {
		// TODO: Show the overlay.
	} else if (m_keys[m_key_bindings.open_chat]) {
		// TODO: Show the chat interface.
	} else if (m_keys[m_key_bindings.show_menu]) {
		// TODO: Show the menu.
	}
}

void GameController::move_objects(float timescale) {
	if (m_players.size() == 0) {
		return;
	}
	
	double player_x_vel = m_players.at(0).get_x_vel() * timescale;
	double player_y_vel = m_players.at(0).get_y_vel() * timescale;
	
	double new_x = m_players.at(0).get_x() + player_x_vel;
	double new_y = m_players.at(0).get_y() + player_y_vel;
	double half_width = m_players.at(0).get_width() / 2;
	double half_height = m_players.at(0).get_height() / 2;
	
	if (new_x - half_width < 0) {
		new_x = half_width;
		m_players.at(0).set_velocity(0, 0);
	} else if (new_x + half_width > m_screen_width) {
		new_x = m_screen_width - half_width;
		m_players.at(0).set_velocity(0, 0);
	}
	
	if (new_y - half_height < 0) {
		new_y = half_height;
		m_players.at(0).set_velocity(0, 0);
	} else if (new_y + half_height > m_screen_height) {
		new_y = m_screen_height - half_height;
		m_players.at(0).set_velocity(0, 0);
	}
	
	m_players.at(0).set_x(new_x);
	m_players.at(0).set_y(new_y);
}

void GameController::attempt_jump() {
	GraphicalPlayer* player = &m_players.at(0);
	
	double x_dist = m_crosshairs->get_x() - player->get_x();
	double y_dist = m_crosshairs->get_y() - player->get_y();
	double x_vel = 6 * cos(atan2(y_dist, x_dist));
	double y_vel = 6 * sin(atan2(y_dist, x_dist));
	
	if (player->get_x() - (player->get_width()/2) <= 5) {
		player->set_x_vel(x_vel);
		player->set_y_vel(y_vel);
	} else if (player->get_x() + (player->get_width()/2) >= m_screen_width - 5) {
		player->set_x_vel(x_vel);
		player->set_y_vel(y_vel);
	}
	
	if (player->get_y() - (player->get_height()/2) <= 5) {
		player->set_x_vel(x_vel);
		player->set_y_vel(y_vel);
	} else if (player->get_y() + (player->get_height()/2) >= m_screen_height - 5) {
		player->set_x_vel(x_vel);
		player->set_y_vel(y_vel);
	}
}

void GameController::connect_to_server(const char* host, unsigned int port) {
	if (!m_network.connect(host, port)) {
		cerr << "Error: Could not connect to server at " << host << ":" << port << endl;
	}
	
	PacketWriter join_request(JOIN_PACKET);
	join_request << m_version;
	join_request << "TestName";
	
	m_network.send_packet(join_request);
}

void GameController::disconnect() {
	PacketWriter leave_request(LEAVE_PACKET);
	leave_request << m_player_id;
	
	m_network.send_packet(leave_request);
	
	m_network.disconnect();
}

void GameController::welcome(PacketReader& reader) {
	string serverversion;
	int playerid;
	char team;
	
	reader >> serverversion >> playerid >> team;
	
	m_player_id = playerid;

	cerr << "Received welcome packet. Version: " << serverversion << ", Player ID: " << playerid << ", Team: " << team << endl;
	
	m_players.clear();
	m_players.push_back(GraphicalPlayer("MyName", m_player_id, team, new_sprite, new_sprite->get_width()/2, new_sprite->get_height()/2));
	
	// TEMPORARY SPRITE CODE
	m_window->register_sprite(new_sprite);
	
	// PUT THESE BACK WHEN THE SERVER SENDS GAME START, ETC.
	//m_players.at(0).set_is_invisible(true);
	//m_players.at(0).set_is_frozen(true);
	
	send_my_player_update();
}

void GameController::player_update(PacketReader& reader) {
	unsigned int player_id;
	long x;
	long y;
	long velocity_x;
	long velocity_y;
	string flags;
	reader >> player_id >> x >> y >> velocity_x >> velocity_y >> flags;
	
	GraphicalPlayer* currplayer = get_player_by_id(player_id);
	if (currplayer == NULL) {
		cerr << "Error: Received update packet for non-existent player " << player_id << endl;
		return;
	}
	
	currplayer->set_position(x, y);
	currplayer->set_velocity(velocity_x, velocity_y);
	
	if (flags.find_first_of('I') == string::npos) {
		currplayer->set_is_invisible(false);
	} else {
		currplayer->set_is_invisible(true);
	}
	
	if (flags.find_first_of('F') == string::npos) {
		currplayer->set_is_frozen(false);
	} else {
		currplayer->set_is_frozen(true);
	}
	
}

GraphicalPlayer* GameController::get_player_by_id(unsigned int player_id) {
	for (unsigned int i = 0; i < m_players.size(); i++) {
		if (m_players.at(i).get_id() == player_id) {
			return &m_players.at(i);
		}
	}
	return NULL;
}

void GameController::send_my_player_update() {
	PacketWriter player_update(PLAYER_UPDATE_PACKET);
	GraphicalPlayer* my_player = &m_players.at(0);
	string flags;
	
	if (my_player->is_invisible()) {
		flags.push_back('I');
	}
	if (my_player->is_frozen()) {
		flags.push_back('F');
	}
	
	player_update << m_player_id << my_player->get_x() << my_player->get_y() << my_player->get_x_vel() 
		<< my_player->get_y_vel() << flags;
		
	m_network.send_packet(player_update);
}

/* EXAMPLE
void GameController::player_update(PacketReader& reader) {
	long	player_id;
	long	x;
	long	y;
	long	velocity_x;
	long	velocity_y;
	reader >> player_id >> x >> y >> velocity_x >> velocity_y;

	PacketWriter		writer(PLAYER_UPDATE_PACKET);
	writer << player_id << x << y << velocity_x << velocity_y;

	network.send_packet(writer);
}
*/

		//controller.player_update(reader);
