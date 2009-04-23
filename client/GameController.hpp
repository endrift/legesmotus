/*
 * GameController.hpp
 *
 * Copyright 2009 - Nathan Partlan, Andrew Ayer, Daniel Schneider, and Jeffrey Pfau
 * 
 */
 
#ifndef LM_CLIENT_GAMECONTROLLER_HPP
#define LM_CLIENT_GAMECONTROLLER_HPP

#include "GameWindow.hpp"
#include "GraphicalMap.hpp"
#include "ClientNetwork.hpp"
#include "TextManager.hpp"
#include "SoundController.hpp"
#include "common/PacketReader.hpp"
#include "GraphicalPlayer.hpp"
#include "Font.hpp"

#include <string>
#include <vector>
#include <map>

class TiledGraphic;

class GameController {
private:
	struct KeyBindings {
		int quit;
		int jump;
		int show_overlay;
		int show_menu;
		int open_chat;
		int send_chat;
	};
	
	const static int MESSAGE_DISPLAY_TIME = 10000;
	
	GameWindow* 	m_window;
	ClientNetwork	m_network;
	TextManager*	m_text_manager;
	SoundController* m_sound_controller;
	Font*		m_font;
	
	std::string	m_name;
	std::string 	m_client_version;
	std::string	m_input_text;
	std::vector<std::pair<Sprite*, int> > m_messages;
	int		m_protocol_number;
	int 		m_screen_width;
	int 		m_screen_height;
	int		m_map_width;
	int		m_map_height;
	int 		m_pixel_depth;
	bool 		m_fullscreen;
	bool		m_quit_game;
	double		m_offset_x;
	double		m_offset_y;
	double		m_mouse_x;
	double		m_mouse_y;
	Uint8*		m_keys;
	KeyBindings	m_key_bindings;
	std::map<int, GraphicalPlayer> m_players;
	unsigned int	m_player_id;
	
	// Do we want to keep it this way?
	unsigned long	m_time_to_unfreeze;
	
	// TEMPORARY SPRITE CODE
	Sprite*		new_sprite;
	Sprite*		new_sprite_b;
	Sprite*		m_crosshairs;
	Sprite*		m_input_bar;

	// TEMPORARY MAP CODE BY ANDREW
	GraphicalMap*	m_map;
	
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
	void		connect_to_server(const char* host, unsigned int port, std::string name);
	void		disconnect();
	void		player_fired(unsigned int player_id, double start_x, double start_y, double direction);
	void		send_player_shot(unsigned int shooter_id, unsigned int hit_player_id);
	void		send_message(std::string message);
	void		display_message(std::string message, double red=1, double green=1, double blue=1);
	
	// Network callbacks:
	void		welcome(PacketReader& reader);
	void		player_update(PacketReader& reader);
	void		announce(PacketReader& reader);
	void		leave(PacketReader& reader);
	void		gun_fired(PacketReader& reader);
	void		player_shot(PacketReader& reader);
	void		message(PacketReader& reader);
	void		gate_lowering(PacketReader& reader);
};

#endif

