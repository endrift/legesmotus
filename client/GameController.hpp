/*
 * GameController.hpp
 *
 * Copyright 2009 - Nathan Partlan, Andrew Ayer, Daniel Schneider, and Jeffrey Pfau
 * 
 */
 
#ifndef LM_CLIENT_GAMECONTROLLER_HPP
#define LM_CLIENT_GAMECONTROLLER_HPP

#include "GameWindow.hpp"
#include "ClientNetwork.hpp"
#include "common/PacketReader.hpp"
#include "GraphicalPlayer.hpp"

#include <vector>

class GameController {
private:
	struct KeyBindings {
		int quit;
		int jump;
		int show_overlay;
		int show_menu;
		int open_chat;
	};
	
	GameWindow* 	m_window;
	ClientNetwork	m_network;
	
	#define m_version "0.0.1";
	int 		m_screen_width;
	int 		m_screen_height;
	int 		m_pixel_depth;
	bool 		m_fullscreen;
	bool		m_quit_game;
	Uint8*		m_keys;
	KeyBindings	m_key_bindings;
	vector<GraphicalPlayer> m_players;
	int		m_player_id;
	
	// TEMPORARY SPRITE CODE
	Sprite*		new_sprite;
	Sprite*		m_crosshairs;
	
	void		init(int width, int height, int depth, bool fullscreen);
	void		process_input();
	GraphicalPlayer* get_player_by_id(unsigned int player_id);
	void		send_my_player_update();
	void		attempt_jump();
public:
	GameController();
	GameController(int width, int height);
	~GameController();
	
	void		run(int lockfps=60);
	void		set_screen_dimensions(int width, int height);
	void		initialize_key_bindings();
	void		parse_key_input();
	void		move_objects(float timescale);
	void		connect_to_server(const char* host, unsigned int port);
	void		disconnect();
	
	// Network callbacks:
	void		welcome(PacketReader& reader);
	void		player_update(PacketReader& reader);
};

#endif

