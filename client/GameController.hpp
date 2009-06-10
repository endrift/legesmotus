/*
 * GameController.hpp
 *
 * This file is part of Leges Motus, a networked, 2D shooter set in zero gravity.
 * 
 * Copyright 2009 Andrew Ayer, Nathan Partlan, Jeffrey Pfau
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

#ifndef LM_CLIENT_GAMECONTROLLER_HPP
#define LM_CLIENT_GAMECONTROLLER_HPP

#include "GameWindow.hpp"
#include "GraphicalMap.hpp"
#include "ClientNetwork.hpp"
#include "TextManager.hpp"
#include "SoundController.hpp"
#include "TableBackground.hpp"
#include "common/PathManager.hpp"
#include "common/PacketReader.hpp"
#include "common/misc.hpp"
#include "GraphicalPlayer.hpp"
#include "Minimap.hpp"
#include "Font.hpp"
#include "ScrollArea.hpp"
#include "ScrollBar.hpp"

#include <string>
#include <vector>
#include <map>

class TiledGraphic;
class IPAddress;

class GameController {
public:
	static const Color		BLUE_COLOR;
	static const Color		RED_COLOR;
	static const Color		GREYED_OUT;
	static const Color		WHITE_COLOR;
private:
	struct KeyBindings {
		int quit;
		int jump;
		int show_overlay;
		int show_menu;
		int open_chat;
		int open_team_chat;
		int open_console;
		int send_chat;
	};
	
	enum {
		SHOW_MENUS = 0,
		GAME_IN_PROGRESS = 1,
		GAME_OVER = 2,
		SHOW_OPTIONS_MENU = 3,
		SHOW_SERVER_BROWSER = 4
	};

	const static int MESSAGE_DISPLAY_TIME;
	const static int FIRING_DELAY;
	const static int SHOT_DISPLAY_TIME;
	const static int MUZZLE_FLASH_LENGTH;
	const static int GATE_WARNING_FLASH_LENGTH;
	const static double FIRING_RECOIL;
	const static double RANDOM_ROTATION_SCALE;
	const static double MINIMAP_SCALE;
	const static int GATE_STATUS_RECT_WIDTH;
	const static int FROZEN_STATUS_RECT_WIDTH;
	const static int DOUBLE_CLICK_TIME;
	
	PathManager& 	m_path_manager;

	GameWindow* 	m_window;
	ClientNetwork	m_network;
	TextManager*	m_text_manager;
	SoundController* m_sound_controller;
	Font*		m_font;
	Font*		m_medium_font;
	Font*		m_menu_font;
	
	std::string	m_name;
	std::string 	m_client_version;
	std::string	m_input_text;
	std::vector<std::pair<Graphic*, unsigned int> > m_messages;
	std::vector<std::pair<Graphic*, unsigned int> > m_shots;
	int		m_protocol_number;
	int 		m_screen_width;
	int 		m_screen_height;
	int		m_map_width;
	int		m_map_height;
	LMPolygon		m_map_polygon;
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
	KeyBindings m_alt_key_bindings;
	std::map<int, GraphicalPlayer> m_players;
	unsigned int	m_player_id;
	bool		m_holding_gate;
	int		m_gate_lower_sounds[2];
	uint64_t	m_last_fired;
	uint64_t	m_last_clicked;
	uint64_t	m_join_sent_time;

	uint64_t	m_time_to_unfreeze;
	uint64_t	m_total_time_frozen;
	
	// TEMPORARY SPRITE CODE
	Sprite*		gun_normal;
	Sprite*		m_gun_fired;
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
	TableBackground* m_input_bar_back;
	TableBackground* m_chat_window_back;
	
	Sprite*		m_shot;

	Sprite*		m_logo;
	
	std::map<std::string, Graphic*> m_main_menu_items;
	
	std::map<std::string, Graphic*> m_options_menu_items;
	
	TableBackground* m_server_browser_background;
	TableBackground* m_server_browser_selection;
	std::vector<TableBackground*> m_server_browser_buttons;
	std::map<std::string, Graphic*> m_server_browser_items;
	std::vector<IPAddress> m_server_list;
	int		m_server_list_count;
	int		m_server_browser_selected_item;
	ScrollBar* 	m_server_browser_scrollbar;
	ScrollArea*	m_server_browser_scrollarea;
	
	bool		m_show_overlay;
	TableBackground* m_overlay_background;
	std::map<std::string, Graphic*> m_overlay_items;
	ScrollBar* 	m_overlay_scrollbar;
	ScrollArea*	m_overlay_scrollarea;
	
	TableBackground* m_red_gate_status_rect;
	TableBackground* m_blue_gate_status_rect;
	TableBackground* m_red_gate_status_rect_back;
	TableBackground* m_blue_gate_status_rect_back;
	Graphic*	m_red_gate_status_text;
	Graphic*	m_blue_gate_status_text;
	
	Graphic*	m_gate_warning;
	uint64_t	m_gate_warning_time;
	
	TableBackground* m_frozen_status_rect;
	TableBackground* m_frozen_status_rect_back;
	Graphic*	m_frozen_status_text;
	
	// MINIMAP CODE BY JEFFREY
	Minimap*	m_minimap;

	// TEMPORARY MAP CODE BY ANDREW
	GraphicalMap*	m_map;

	// Server Browsing/Scanning
	IPAddress	m_metaserver_address;
	uint32_t	m_current_scan_id;
	
	void		init(GameWindow* window);
	void		process_input();
	GraphicalPlayer* get_player_by_id(unsigned int player_id);
	GraphicalPlayer* get_player_by_name(const char* name);
	void		send_my_player_update();
	void		attempt_jump();

	// Scan a particular server:
	void		scan_server(const IPAddress& server_address);

	// Scan both the local network and the meta server for servers:
	void		scan_all();

	// Scan the local network for servers:
	void		scan_local_network();

	// Connect to the meta server to scan the Internet
	void		contact_metaserver();

	void		send_ack(const PacketReader& packet);
public:
	explicit GameController(PathManager& pathman);
	GameController(PathManager& pathman, int width, int height, bool fullscreen =false, int depth =24);
	~GameController();
	
	void		run(int lockfps=60);
	void		set_screen_dimensions(int width, int height);
	void		initialize_key_bindings();
	void		parse_key_input();
	void		move_objects(float timescale);
	void		connect_to_server(const IPAddress& server_address, char team =0);
	void		connect_to_server(int servernum);
	void		disconnect();
	void		player_fired(unsigned int player_id, double start_x, double start_y, double direction);
	void		send_player_shot(unsigned int shooter_id, unsigned int hit_player_id, double angle);
	void		send_message(std::string message);
	void		send_team_message(std::string message);
	void		display_message(std::string message) { display_message(message, Color::WHITE); }
	void		display_message(std::string message, Color);
	void		send_gate_hold(bool holding);
	void		set_players_visible(bool visible);
	void		process_mouse_click(SDL_Event event);
	void		send_animation_packet(std::string sprite, std::string field, int value);
	void		send_name_change_packet(const char* new_name);
	void		send_team_change_packet(char new_team);
	void		toggle_score_overlay(bool visible);
	void		toggle_server_browser(bool visible);
	void		delete_server_browser_entry(int num);
	void		change_team_scores(int bluescore, int redscore);
	void 		update_individual_scores();
	void 		update_individual_score_line(int count, const GraphicalPlayer& currplayer);
	void		delete_individual_score(const GraphicalPlayer& currplayer);
	void		set_player_name(std::string name);
	void		clear_players();
	std::string	format_time_from_millis(uint64_t milliseconds);
	
	// Network callbacks:
	void		welcome(PacketReader& reader);
	void		player_update(PacketReader& reader);
	void		announce(PacketReader& reader);
	void		leave(PacketReader& reader);
	void		gun_fired(PacketReader& reader);
	void		player_shot(PacketReader& reader);
	void		message(PacketReader& reader);
	void		gate_update(PacketReader& reader);
	void		game_start(PacketReader& reader);
	void		game_stop(PacketReader& reader);
	void		score_update(PacketReader& reader);
	void		animation_packet(PacketReader& reader);
	void		request_denied(PacketReader& reader);
	void		name_change(PacketReader& reader);
	void		team_change(PacketReader& reader);
	void		server_info(const IPAddress& server_address, PacketReader& reader);
};

#endif

