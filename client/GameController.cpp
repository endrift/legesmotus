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
	// TEMPORARY SPRITE CODE
	delete new_sprite;
	delete new_sprite_b;
	delete m_crosshairs;

	// TEMPORARY MAP CODE BY ANDREW
	delete m_map;

	delete m_text_manager;
	delete m_sound_controller;
	delete m_font;

	// The GameWindow instance should always be destroyed last, since other stuff may depend on it.
	m_window->destroy_instance();
}

void GameController::init(int width, int height, int depth, bool fullscreen) {
	srand ( time(NULL) );
	
	initialize_key_bindings();
	
	m_screen_width = width;
	m_screen_height = height;
	
	m_input_text = "> ";
	m_input_bar = NULL;
	
	m_client_version = "0.0.2";
	m_protocol_number = 1;
	
	m_pixel_depth = depth;
	m_fullscreen = fullscreen;
	m_quit_game = false;
	m_window = GameWindow::get_instance(m_screen_width, m_screen_height, m_pixel_depth, m_fullscreen);

	m_time_to_unfreeze = 0;

	m_font = new Font("data/fonts/JuraMedium.ttf", 12);
	m_text_manager = new TextManager(m_font);
	
	m_sound_controller = new SoundController();

	// TEMPORARY MAP CODE BY ANDREW
	m_map = new GraphicalMap(m_window);
	m_map->load_file("data/maps/test.map");
	m_map_width = m_map->get_width();
	m_map_height = m_map->get_height();

	// TEMPORARY SPRITE CODE
	new_sprite = new Sprite("data/sprites/blue_full.png");
	new_sprite_b = new Sprite("data/sprites/red_full.png");
	m_crosshairs = new Sprite("data/sprites/crosshairs.png");
	m_crosshairs->set_priority(-1);
	m_window->register_hud_graphic(m_crosshairs);
}

void GameController::run(int lockfps) {
	cerr << "SDL window is: " << m_window->get_width() << " pixels wide and " 
		<< m_window->get_height() << " pixels tall." << endl;
	
	unsigned long startframe = SDL_GetTicks();
	unsigned long lastmoveframe = startframe;
	
	/* 1 second / FPS = milliseconds per frame */
	double delay = 1000.0 / lockfps;
	
	display_message("Welcome to Leges Motus!");
	
	while(m_quit_game == false) {
		process_input();
		
		m_network.receive_packets(*this);
		
		if (m_quit_game == true) {
			break;
		}
		
		if (!m_players.empty() && m_time_to_unfreeze < SDL_GetTicks() && m_time_to_unfreeze != 0) {
			m_sound_controller->play_sound("unfreeze");
			m_players[m_player_id].set_is_frozen(false);
			m_time_to_unfreeze = 0;
		}
		
		// Update graphics if frame rate is correct
		unsigned long currframe = SDL_GetTicks();
		if((currframe - startframe) >= delay) {
			for (unsigned int i = 0; i < m_messages.size(); i++) {
				if (m_messages[i].second < currframe) {
					m_text_manager->remove_string(m_messages[i].first, m_window);
					m_messages.erase(m_messages.begin() + i);
				}
			}
			
			for (unsigned int i = 0; i < m_messages.size(); i++) {
				int y = 20 + (m_font->ascent() + m_font->descent() + 5) * i;
				m_text_manager->reposition_string(m_messages[i].first, 20, y, TextManager::LEFT);
			}
				
			move_objects((SDL_GetTicks() - lastmoveframe) / delay); // scale all position changes to keep game speed constant. 
		
			lastmoveframe = SDL_GetTicks();
			
			// the framerate:
			int framerate = (1000/(currframe - startframe));
			
			send_my_player_update();
			
			if (!m_players.empty()) {
				m_offset_x = m_players[m_player_id].get_x() - (m_screen_width/2.0);
				m_offset_y = m_players[m_player_id].get_y() - (m_screen_height/2.0);
				m_window->set_offset_x(m_offset_x);
				m_window->set_offset_y(m_offset_y);
			
				m_crosshairs->set_x(m_mouse_x);
				m_crosshairs->set_y(m_mouse_y);
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
				if (event.key.keysym.sym == m_key_bindings.quit) {
					if (m_input_bar != NULL) {
						SDL_EnableUNICODE(0);
						m_text_manager->remove_string(m_input_bar, m_window);
						m_input_bar = NULL;
						m_input_text = "> ";
					} else {
						cerr << "Quit key pressed - quitting." << endl;
						m_quit_game = true;
					}
				}
				
				if (m_input_bar != NULL) {
					if (event.key.keysym.sym == m_key_bindings.send_chat) {
						send_message(m_input_text.substr(2));
					
						SDL_EnableUNICODE(0);
						m_text_manager->remove_string(m_input_bar, m_window);
						m_input_bar = NULL;
						m_input_text = "> ";
					} else if (event.key.keysym.sym == SDLK_BACKSPACE) {
						m_input_text.erase(m_input_text.length() - 1);
						m_text_manager->remove_string(m_input_bar, m_window);
						m_input_bar = m_text_manager->place_string(m_input_text, 20, m_screen_height-100, TextManager::LEFT, TextManager::LAYER_HUD, m_window);
					} else {
						if ( (event.key.keysym.unicode & 0xFF80) == 0 && event.key.keysym.unicode != 0) {
							m_input_text.push_back(event.key.keysym.unicode & 0x7F);
						} else {
							// INTERNATIONAL CHARACTER... DO SOMETHING.
						}
						m_text_manager->remove_string(m_input_bar, m_window);
						m_input_bar = m_text_manager->place_string(m_input_text, 20, m_screen_height-100, TextManager::LEFT, TextManager::LAYER_HUD, m_window);
					}
				} else {
					//Check which key using: event.key.keysym.sym == SDLK_<SOMETHING>
					if (event.key.keysym.sym == m_key_bindings.jump) {
						attempt_jump();
					} else if (event.key.keysym.sym == m_key_bindings.show_overlay) {
						// TODO: Show the overlay.
					} else if (event.key.keysym.sym == m_key_bindings.open_chat) {
						SDL_EnableUNICODE(1);
						m_text_manager->set_active_color(1, 1, 1);
						if (m_input_bar == NULL) {
							m_input_bar = m_text_manager->place_string("> ", 20, m_screen_height-100, TextManager::LEFT, TextManager::LAYER_HUD, m_window);
						}
					} else if (event.key.keysym.sym == m_key_bindings.show_menu) {
						// TODO: Show the menu.
					}
				}
				break;

			case SDL_KEYUP:
				break;
				
			case SDL_MOUSEMOTION:
				// Send motion to the GameWindow, telling it where the mouse is.
				// Use: event.motion.xrel, event.motion.yrel (changes in position), event.motion.x, event.motion.y
				m_mouse_x = event.motion.x;
				m_mouse_y = event.motion.y;
				m_crosshairs->set_x(m_mouse_x);
				m_crosshairs->set_y(m_mouse_y);
				break;
				
			case SDL_MOUSEBUTTONDOWN:
				// Firing code, use event.button.button, event.button.x, event.button.y
				// TODO relocate to function
				if (event.button.button == 1) {
					if (m_players.empty() || m_players[m_player_id].is_frozen()) {
						return;
					}
					double x_dist = (event.button.x + m_offset_x) - m_players[m_player_id].get_x();
					double y_dist = (event.button.y + m_offset_y) - m_players[m_player_id].get_y();
					double direction = atan2(y_dist, x_dist) * RADIANS_TO_DEGREES;
					player_fired(m_player_id, m_players[m_player_id].get_x(), m_players[m_player_id].get_y(), direction);
					m_sound_controller->play_sound("fire");
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
	m_key_bindings.open_chat = SDLK_t;
	m_key_bindings.send_chat = SDLK_RETURN;
}

void GameController::parse_key_input() {
	// For keys that can be held down:
   	m_keys = SDL_GetKeyState(NULL);
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
	
	bool holdinggate = false;
	
	const list<MapObject>& map_objects(m_map->get_objects());
	list<MapObject>::const_iterator thisobj;
	int radius = m_players[m_player_id].get_radius();
	Point currpos = Point(new_x, new_y);
	Point oldpos = Point(m_players[m_player_id].get_x(), m_players[m_player_id].get_y());
	//cerr << "Start: " << SDL_GetTicks() << endl;
	for (thisobj = map_objects.begin(); thisobj != map_objects.end(); thisobj++) {
		if (thisobj->get_sprite() == NULL) {
			continue;
		}
		const Polygon& poly(thisobj->get_bounding_polygon());
		double newdist = poly.intersects_circle(currpos, radius);
		double olddist = poly.intersects_circle(oldpos, radius);
		if (newdist != -1) {
			if (newdist < olddist) {
				//cerr << "New dist: " << newdist << " Old dist: " << olddist << endl;
				//cerr << "Hitting object" << endl;
				if (thisobj->get_type() == Map::OBSTACLE) {
					m_players[m_player_id].set_velocity(0, 0);
					new_x = m_players[m_player_id].get_x();
					new_y = m_players[m_player_id].get_y();
				}
			}
			if (thisobj->get_type() == Map::GATE && !m_players[m_player_id].is_frozen()) {
				if (!m_holding_gate) {
					send_gate_hold(true);
				}
				m_holding_gate = true;
				holdinggate = true;
			}
		}
	}
	//cerr << "End: " << SDL_GetTicks() << endl;
	
	if (!holdinggate) {
		if (m_holding_gate) {
			send_gate_hold(false);
		}
		m_holding_gate = false;
	}
	
	//m_text_manager->reposition_string(m_players[m_player_id].get_name_sprite(), new_x, new_y, TextManager::CENTER);
	m_players[m_player_id].set_x(new_x);
	m_players[m_player_id].set_y(new_y);
	
	map<int, GraphicalPlayer>::iterator it;
	for ( it=m_players.begin() ; it != m_players.end(); it++ ) {
		GraphicalPlayer currplayer = (*it).second;
		if (currplayer.is_invisible()) {
			m_text_manager->reposition_string(currplayer.get_name_sprite(), -1000, -1000, TextManager::CENTER); // MOVE STRING OFF SCREEN
		} else {
			m_text_manager->reposition_string(currplayer.get_name_sprite(), currplayer.get_x(), currplayer.get_y() - currplayer.get_height()/2, TextManager::CENTER);
		}
	}
}

void GameController::attempt_jump() {
	if (m_players.empty()) {
		return;
	}
	
	if (m_players[m_player_id].is_frozen()) {
		return;
	}
	
	GraphicalPlayer* player = &m_players[m_player_id];
	
	double x_dist = (m_crosshairs->get_x() + m_offset_x) - player->get_x();
	double y_dist = (m_crosshairs->get_y() + m_offset_y) - player->get_y();
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

	list<MapObject>::const_iterator thisobj;
	const list<MapObject>& map_objects(m_map->get_objects());
	Point currpos = Point(player->get_x(), player->get_y());
	for (thisobj = map_objects.begin(); thisobj != map_objects.end(); thisobj++) {
		if (thisobj->get_sprite() == NULL) {
			continue;
		}
		if (thisobj->get_type() != Map::OBSTACLE) {
			// Only obstacles can be jumped from
			continue;
		}
		const Polygon& poly(thisobj->get_bounding_polygon());
		double newdist = poly.intersects_circle(currpos, player->get_radius());
		if (newdist != -1) {
			player->set_x_vel(x_vel);
			player->set_y_vel(y_vel);
		}
	}
}

void GameController::player_fired(unsigned int player_id, double start_x, double start_y, double direction) {
	if (player_id == m_player_id) {
		map<int, GraphicalPlayer>::iterator it;
		for ( it=m_players.begin() ; it != m_players.end(); it++ ) {
			GraphicalPlayer currplayer = (*it).second;
			if (currplayer.get_id() == player_id || currplayer.is_frozen()) {
				continue;
			}
			double playerdist = dist_between_points(start_x, start_y, currplayer.get_x(), currplayer.get_y());
			int end_x = start_x + playerdist * cos(direction * DEGREES_TO_RADIANS);
			int end_y = start_y + playerdist * sin(direction * DEGREES_TO_RADIANS);
			vector<double> closestpoint = closest_point_on_line(start_x, start_y, end_x, end_y, currplayer.get_x(), currplayer.get_y());
			
			if (closestpoint.size() == 0) {
				continue;
			}
			
			double dist = dist_between_points(currplayer.get_x(), currplayer.get_y(), closestpoint.at(0), closestpoint.at(1));
			
			// If the closest point was behind the beginning of the shot, it's not a hit.
			if (closestpoint.at(2) < 0) {
				continue;
			}
			
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

void GameController::connect_to_server(const char* host, unsigned int port, string name) {
	if (!m_network.connect(host, port)) {
		cerr << "Error: Could not connect to server at " << host << ":" << port << endl;
	}
	
	PacketWriter join_request(JOIN_PACKET);
	join_request << m_protocol_number;
	if (name.empty()) {
		name = "MyName";
	}
	m_name = name;
	join_request << name;
	int team = rand() % 2;
	if (team == 0) {
		join_request << 'A';
	} else {
		join_request << 'B';
	}
	
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
	
	if (team == 'A') {
		m_players.insert(pair<int, GraphicalPlayer>(m_player_id,GraphicalPlayer(m_name.c_str(), m_player_id, team, new_sprite, new_sprite->get_width()/2, new_sprite->get_height()/2)));
		m_text_manager->set_active_color(0.0, 0.0, 0.7);
		m_window->register_graphic(new_sprite);
	} else {
		m_players.insert(pair<int, GraphicalPlayer>(m_player_id,GraphicalPlayer(m_name.c_str(), m_player_id, team, new_sprite_b, new_sprite_b->get_width()/2, new_sprite_b->get_height()/2)));
		m_text_manager->set_active_color(0.7, 0.0, 0.0);
		m_window->register_graphic(new_sprite_b);
	}
	
	m_players[m_player_id].set_radius(30);
	m_players[m_player_id].set_name_sprite(m_text_manager->place_string(m_players[m_player_id].get_name(), m_screen_width/2, (m_screen_height/2)-m_players[m_player_id].get_height()/2, TextManager::CENTER, TextManager::LAYER_MAIN, m_window));
	
	// REMOVE THESE WHEN THE SERVER SENDS GAME START, ETC.
	m_players[m_player_id].set_is_invisible(false);
	m_players[m_player_id].set_is_frozen(false);
	
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
	
	if (team == 'A') {
		m_players.insert(pair<int, GraphicalPlayer>(playerid,GraphicalPlayer((const char*)playername.c_str(), playerid, team, new Sprite(*new_sprite))));
		m_text_manager->set_active_color(0.0, 0.0, 0.6);
	} else {
		m_players.insert(pair<int, GraphicalPlayer>(playerid,GraphicalPlayer((const char*)playername.c_str(), playerid, team, new Sprite(*new_sprite_b))));
		m_text_manager->set_active_color(0.6, 0.0, 0.0);
	}
	// TEMPORARY SPRITE CODE
	m_window->register_graphic(m_players[playerid].get_sprite());
	m_players[playerid].set_name_sprite(m_text_manager->place_string(m_players[playerid].get_name(), m_players[playerid].get_x(), m_players[playerid].get_y()-m_players[playerid].get_height()/2, TextManager::CENTER, TextManager::LAYER_MAIN, m_window));
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
	
	GraphicalPlayer* currplayer = get_player_by_id(player_id);
	if (currplayer == NULL) {
		cerr << "Error: Received update packet for non-existent player " << player_id << endl;
		return;
	}
	
	currplayer->set_position(x, y);
	currplayer->set_velocity(velocity_x, velocity_y);
	
	
	if (flags.find_first_of('I') == string::npos) {
		currplayer->set_is_invisible(false);
		m_text_manager->reposition_string(m_players[player_id].get_name_sprite(), x, y - m_players[player_id].get_height()/2, TextManager::CENTER);
	} else {
		currplayer->set_is_invisible(true);
		m_text_manager->reposition_string(m_players[player_id].get_name_sprite(), -1000, -1000, TextManager::CENTER); // MOVE STRING OFF SCREEN
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

void GameController::send_message(string message) {
	PacketWriter message_writer(MESSAGE_PACKET);
	// TODO: Add recipient before message.
	message_writer << m_player_id << "" << message;
	m_network.send_packet(message_writer);
}

void GameController::leave(PacketReader& reader) {
	unsigned int playerid;
	reader >> playerid;
	
	m_text_manager->remove_string(m_players[playerid].get_name_sprite(), m_window);
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
	
	m_sound_controller->play_sound("fire");
	player_fired(playerid, start_x, start_y, rotation);
}

void GameController::player_shot(PacketReader& reader) {
	unsigned int shooter_id;
	unsigned int shot_id;
	unsigned long time_to_unfreeze;
	
	reader >> shooter_id >> shot_id >> time_to_unfreeze;
	
	if (shot_id == m_player_id) {
		m_sound_controller->play_sound("freeze");
		cerr << "I was hit! Time to unfreeze: " << time_to_unfreeze << endl;
		m_players[m_player_id].set_is_frozen(true);
		m_time_to_unfreeze = SDL_GetTicks() + time_to_unfreeze;
	}
}

void GameController::message(PacketReader& reader) {
	unsigned int sender_id;
	string recipient;
	string message_text;
	
	reader >> sender_id >> recipient >> message_text;
	
	string message = m_players[sender_id].get_name();
	message.append(": ");
	message.append(message_text);
	
	char team = m_players[sender_id].get_team();
	if (team == 'A') {
		display_message(message, 0.0, 0.0, 1);
	} else {
		display_message(message, 1, 0.0, 0.0);
	}
}

void GameController::gate_lowering(PacketReader& reader) {
	uint32_t	lowering_player_id; 	// Who's lowering the gate?
	char		team;			// Which team's gate is being lowered
	double		progress;		// How much has the gate gone down? 0 == not at all .. 1 == all the way
	reader >> lowering_player_id >> team >> progress;
	
	if (progress < .05) {
		m_sound_controller->play_sound("gatelower");
	}
	
	cerr << "Progress: " << progress << endl;
	m_map->set_gate_progress(team, progress);

	// TODO: use the player id to display a HUD message or something...
}

void GameController::send_gate_hold(bool holding) {
	PacketWriter gate_hold(GATE_LOWERING_PACKET);
	if (holding) {
		cerr << "Sending: hold" << endl;
		gate_hold << m_player_id << m_players[m_player_id].get_team() << 1;
	} else {
		cerr << "Sending: release" << endl;
		gate_hold << m_player_id << m_players[m_player_id].get_team() << 0;
	}
	m_network.send_packet(gate_hold);
}

void GameController::display_message(string message, double red, double green, double blue) {
	m_text_manager->set_active_color(red, green, blue);
	int y = 20 + (m_font->ascent() + m_font->descent() + 5) * m_messages.size();
	Sprite* message_sprite = m_text_manager->place_string(message, 20, y, TextManager::LEFT, TextManager::LAYER_HUD, m_window);
	m_messages.push_back(pair<Sprite*, int>(message_sprite, SDL_GetTicks() + MESSAGE_DISPLAY_TIME));
}

GraphicalPlayer* GameController::get_player_by_id(unsigned int player_id) {
	map<int, GraphicalPlayer>::iterator it(m_players.find(player_id));
	return it == m_players.end() ? NULL : &it->second;
}

