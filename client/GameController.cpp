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
#include "Sprite.hpp"
#include "TiledGraphic.hpp"
#include "common/PacketReader.hpp"
#include "common/PacketWriter.hpp"
#include "common/network.hpp"
#include "common/math.hpp"

#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

using namespace std;

GameController::GameController() {
	init(1024, 768, 24, false);
}

GameController::GameController(int width, int height) {
	init(width, height, 24, true);
}

GameController::~GameController() {
	m_window->destroy_instance();
	
	// TEMPORARY SPRITE CODE
	delete new_sprite;
	delete m_crosshairs;
	delete m_background;
}

void GameController::init(int width, int height, int depth, bool fullscreen) {
	initialize_key_bindings();
	
	m_screen_width = width;
	m_screen_height = height;
	
	m_client_version = "0.0.1";
	m_protocol_number = 1;
	
	// TEMPORARY MAP WIDTH AND HEIGHT
	m_map_width = 500;
	m_map_height = 500;
	
	m_pixel_depth = depth;
	m_fullscreen = fullscreen;
	m_quit_game = false;
	m_window = GameWindow::get_instance(m_screen_width, m_screen_height, m_pixel_depth, m_fullscreen);

	m_time_to_unfreeze = 0;

	// TEMPORARY SPRITE CODE
	new_sprite = new Sprite("data/sprites/blue_full.png");
	m_crosshairs = new Sprite("data/sprites/crosshairs.png");
	m_crosshairs->set_priority(-1);
	m_window->register_graphic(m_crosshairs);
	m_background = new TiledGraphic("data/sprites/blue_bgtile.png");
	m_background->set_start_x(0);
	m_background->set_start_y(0);
	m_background->set_width(m_map_width);
	m_background->set_height(m_map_height);
	m_background->set_priority(1);
	m_window->register_graphic(m_background);
}

void GameController::run(int lockfps) {
	cerr << "SDL window is: " << m_window->get_width() << " pixels wide and " 
		<< m_window->get_height() << " pixels tall." << endl;
	
	unsigned long startframe = SDL_GetTicks();
	unsigned long lastmoveframe = startframe;
	
	/* 1 second / FPS = milliseconds per frame */
	double delay = 1000.0 / lockfps;
	
	while(m_quit_game == false) {
		process_input();
		
		m_network.receive_packets(*this);
		
		if (m_quit_game == true) {
			break;
		}
		
		if (!m_players.empty() && m_time_to_unfreeze < SDL_GetTicks()) {
			m_players[m_player_id].set_is_frozen(false);
		}
		
		move_objects((SDL_GetTicks() - lastmoveframe) / delay); // scale all position changes to keep game speed constant. 
		
		lastmoveframe = SDL_GetTicks();
		
		// Update graphics if frame rate is correct
		unsigned long currframe = lastmoveframe;
		if((currframe - startframe) >= delay) {
			// the framerate:
			int framerate = (1000/(currframe - startframe));
			
			send_my_player_update();
			
			if (!m_players.empty()) {
				m_offset_x = m_players[m_player_id].get_x() - (m_screen_width/2.0);
				m_offset_y = m_players[m_player_id].get_y() - (m_screen_height/2.0);
				m_window->set_offset_x(m_offset_x);
				m_window->set_offset_y(m_offset_y);
			
				m_crosshairs->set_x(m_mouse_x + m_offset_x);
				m_crosshairs->set_y(m_mouse_y + m_offset_y);
			}
			
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
				m_mouse_x = event.motion.x;
				m_mouse_y = event.motion.y;
				m_crosshairs->set_x(m_mouse_x + m_offset_x);
				m_crosshairs->set_y(m_mouse_y + m_offset_y);
				break;
				
			case SDL_MOUSEBUTTONDOWN:
				// Firing code, use event.button.button, event.button.x, event.button.y
				if (event.button.button == 1) {
					if (m_players.empty() | m_players[m_player_id].is_frozen()) {
						return;
					}
					double x_dist = (event.button.x + m_offset_x) - m_players[m_player_id].get_x();
					double y_dist = (event.button.y + m_offset_y) - m_players[m_player_id].get_y();
					double direction = atan2(y_dist, x_dist) * RADIANS_TO_DEGREES;
					player_fired(m_player_id, m_players[m_player_id].get_x(), m_players[m_player_id].get_y(), direction);
				}
				break;
				
			case SDL_MOUSEBUTTONUP:
				break;
				
			case SDL_QUIT:
				m_quit_game = true;
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
	if (m_players.empty()) {
		return;
	}
	
	double player_x_vel = m_players[m_player_id].get_x_vel() * timescale;
	double player_y_vel = m_players[m_player_id].get_y_vel() * timescale;
	
	double new_x = m_players[m_player_id].get_x() + player_x_vel;
	double new_y = m_players[m_player_id].get_y() + player_y_vel;
	double half_width = m_players[m_player_id].get_width() / 2;
	double half_height = m_players[m_player_id].get_height() / 2;
	
	if (new_x - half_width < 0) {
		new_x = half_width;
		m_players[m_player_id].set_velocity(0, 0);
	} else if (new_x + half_width > m_map_width) {
		new_x = m_map_width - half_width;
		m_players[m_player_id].set_velocity(0, 0);
	}
	
	if (new_y - half_height < 0) {
		new_y = half_height;
		m_players[m_player_id].set_velocity(0, 0);
	} else if (new_y + half_height > m_map_height) {
		new_y = m_map_height - half_height;
		m_players[m_player_id].set_velocity(0, 0);
	}
	
	m_players[m_player_id].set_x(new_x);
	m_players[m_player_id].set_y(new_y);
}

void GameController::attempt_jump() {
	if (m_players.empty()) {
		return;
	}
	
	if (m_players[m_player_id].is_frozen()) {
		return;
	}
	
	GraphicalPlayer* player = &m_players[m_player_id];
	
	double x_dist = m_crosshairs->get_x() - player->get_x();
	double y_dist = m_crosshairs->get_y() - player->get_y();
	double x_vel = 6 * cos(atan2(y_dist, x_dist));
	double y_vel = 6 * sin(atan2(y_dist, x_dist));
	
	if (player->get_x() - (player->get_width()/2) <= 5) {
		player->set_x_vel(x_vel);
		player->set_y_vel(y_vel);
	} else if (player->get_x() + (player->get_width()/2) >= m_map_width - 5) {
		player->set_x_vel(x_vel);
		player->set_y_vel(y_vel);
	}
	
	if (player->get_y() - (player->get_height()/2) <= 5) {
		player->set_x_vel(x_vel);
		player->set_y_vel(y_vel);
	} else if (player->get_y() + (player->get_height()/2) >= m_map_height - 5) {
		player->set_x_vel(x_vel);
		player->set_y_vel(y_vel);
	}
}

void GameController::player_fired(unsigned int player_id, double start_x, double start_y, double direction) {
	if (player_id == m_player_id) {
		map<int, GraphicalPlayer>::iterator it;
		for ( it=m_players.begin() ; it != m_players.end(); it++ ) {
			GraphicalPlayer currplayer = (*it).second;
			if (currplayer.get_id() == player_id) {
				continue;
			}
			double playerdist = dist_between_points(start_x, start_y, currplayer.get_x(), currplayer.get_y());
			int end_x = start_x + playerdist * cos(direction * DEGREES_TO_RADIANS);
			int end_y = start_y + playerdist * sin(direction * DEGREES_TO_RADIANS);
			vector<int> closestpoint = closest_point_on_line(start_x, start_y, end_x, end_y, currplayer.get_x(), currplayer.get_y());
			double dist = dist_between_points(currplayer.get_x(), currplayer.get_y(), closestpoint.at(0), closestpoint.at(1));
			
			// If the shot hit the player:
			if (dist < currplayer.get_radius()) {
				send_player_shot(player_id, currplayer.get_id());
			}
		}
	
		PacketWriter gun_fired(GUN_FIRED_PACKET);
		gun_fired << player_id;
		gun_fired << start_x;
		gun_fired << start_y;
		gun_fired << direction;
		
		m_network.send_packet(gun_fired);
	}
}

void GameController::send_player_shot(unsigned int shooter_id, unsigned int hit_player_id) {
	PacketWriter player_shot(PLAYER_SHOT_PACKET);
	player_shot << shooter_id;
	player_shot << hit_player_id;
	
	m_network.send_packet(player_shot);
}

void GameController::connect_to_server(const char* host, unsigned int port) {
	if (!m_network.connect(host, port)) {
		cerr << "Error: Could not connect to server at " << host << ":" << port << endl;
	}
	
	PacketWriter join_request(JOIN_PACKET);
	join_request << m_protocol_number;
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
	m_players.insert(pair<int, GraphicalPlayer>(m_player_id,GraphicalPlayer("MyName", m_player_id, team, new_sprite, new_sprite->get_width()/2, new_sprite->get_height()/2)));
	m_players[m_player_id].set_radius(50);
	
	// TEMPORARY SPRITE CODE
	m_window->register_graphic(new_sprite);
	
	// PUT THESE BACK WHEN THE SERVER SENDS GAME START, ETC.
	//m_players[m_player_id].set_is_invisible(true);
	//m_players[m_player_id].set_is_frozen(true);
	
	send_my_player_update();
}

void GameController::announce(PacketReader& reader) {
	unsigned int playerid;
	string playername;
	char team;
	
	reader >> playerid >> playername >> team;
	if (playerid == m_player_id) {
		return;
	}
	
	// TEMPORARY SPRITE CODE
	m_players.insert(pair<int, GraphicalPlayer>(playerid,GraphicalPlayer((const char*)playername.c_str(), playerid, team, new Sprite(*new_sprite))));
	m_window->register_graphic(m_players[playerid].get_sprite());
	m_players[playerid].set_radius(50);
}

void GameController::player_update(PacketReader& reader) {
	unsigned int player_id;
	long x;
	long y;
	long velocity_x;
	long velocity_y;
	string flags;
	reader >> player_id >> x >> y >> velocity_x >> velocity_y >> flags;
	
	GraphicalPlayer* currplayer = &m_players[player_id];
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

void GameController::send_my_player_update() {
	PacketWriter player_update(PLAYER_UPDATE_PACKET);
	if (m_players.empty()) {
		return;
	}
	
	GraphicalPlayer* my_player = &m_players[m_player_id];
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

void GameController::leave(PacketReader& reader) {
	unsigned int playerid;
	reader >> playerid;
	
	m_window->unregister_graphic(m_players[playerid].get_sprite());
	delete m_players[playerid].get_sprite();
	m_players.erase(playerid);
}

void GameController::gun_fired(PacketReader& reader) {
	unsigned int playerid;
	double start_x;
	double start_y;
	double rotation;
	reader >> playerid >> start_x >> start_y >> rotation;
	
	//cerr << "Received gun_fired" << endl;
	
	if (playerid == m_player_id) {
		return;
	}
	
	player_fired(playerid, start_x, start_y, rotation);
}

void GameController::player_shot(PacketReader& reader) {
	unsigned int shooter_id;
	unsigned int shot_id;
	unsigned long time_to_unfreeze;
	
	reader >> shooter_id >> shot_id >> time_to_unfreeze;
	
	if (shot_id == m_player_id) {
		cerr << "I was hit! Time to unfreeze: " << time_to_unfreeze << endl;
		m_players[m_player_id].set_is_frozen(true);
		m_time_to_unfreeze = SDL_GetTicks() + time_to_unfreeze * 1000;
	}
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
