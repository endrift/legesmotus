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
#include "common/PathManager.hpp"
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
	
	enum {
		SHOW_MENUS = 0,
		GAME_IN_PROGRESS = 1,
		GAME_OVER = 2
	};

	const static int MESSAGE_DISPLAY_TIME = 10000;
	const static int FIRING_DELAY = 700;
	
	GameWindow* 	m_window;
	ClientNetwork	m_network;
	TextManager*	m_text_manager;
	SoundController* m_sound_controller;
	Font*		m_font;
	Font*		m_menu_font;
	
	std::string	m_name;
	std::string 	m_client_version;
	std::string	m_input_text;
	std::vector<std::pair<Graphic*, unsigned int> > m_messages;
	int		m_protocol_number;
	int 		m_screen_width;
	int 		m_screen_height;
	int		m_map_width;
	int		m_map_height;
	int 		m_pixel_depth;
	int		m_game_state;
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
	bool		m_holding_gate;
	long int	m_last_fired;
	
	// Do we want to keep it this way?
	unsigned long	m_time_to_unfreeze;
	
	PathManager* 	m_path_manager;
	
	// TEMPORARY SPRITE CODE
	Sprite*		gun_normal;
	GraphicGroup 	blue_player;
	GraphicGroup	blue_arm_gun;
	Sprite*		blue_sprite;
	Sprite*		blue_front_arm;
	Sprite*		blue_back_arm;
	GraphicGroup 	red_player;
	GraphicGroup	red_arm_gun;
	Sprite*		red_sprite;
	Sprite*		red_front_arm;
	Sprite*		red_back_arm;
	Sprite*		m_crosshairs;
	Graphic*	m_input_bar;
	
	Sprite*		m_shot;

	Sprite*		m_logo;
	std::map<std::string, Graphic*> m_main_menu_items;

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
	void		connect_to_server(const char* host, unsigned int port, std::string name, char team);
	void		disconnect();
	void		player_fired(unsigned int player_id, double start_x, double start_y, double direction);
	void		send_player_shot(unsigned int shooter_id, unsigned int hit_player_id);
	void		send_message(std::string message);
	void		display_message(std::string message, double red=1, double green=1, double blue=1);
	void		send_gate_hold(bool holding);
	void		set_players_visible(bool visible);
	void		process_mouse_click(SDL_Event event);
	void		send_animation_packet(std::string sprite, std::string field, int value);
	
	// Network callbacks:
	void		welcome(PacketReader& reader);
	void		player_update(PacketReader& reader);
	void		announce(PacketReader& reader);
	void		leave(PacketReader& reader);
	void		gun_fired(PacketReader& reader);
	void		player_shot(PacketReader& reader);
	void		message(PacketReader& reader);
	void		gate_lowering(PacketReader& reader);
	void		game_start(PacketReader& reader);
	void		game_stop(PacketReader& reader);
	void		score_update(PacketReader& reader);
	void		animation_packet(PacketReader& reader);
};

#endif

