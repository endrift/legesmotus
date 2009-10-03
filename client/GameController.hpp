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
#include "ServerBrowser.hpp"
#include "ClientConfiguration.hpp"
#include "TransitionManager.hpp"
#include "ChatLog.hpp"
#include "common/PathManager.hpp"
#include "common/PacketReader.hpp"
#include "common/misc.hpp"
#include "common/GameParameters.hpp"
#include "common/WeaponReader.hpp"
#include "common/Polygon.hpp"
#include "GraphicalPlayer.hpp"
#include "Radar.hpp"
#include "Font.hpp"
#include "ScrollArea.hpp"
#include "ScrollBar.hpp"
#include "MapReceiver.hpp"
#include "ArbitraryMenu.hpp"
#include "TextMenuItem.hpp"
#include "ListMenuItem.hpp"
#include "Form.hpp"
#include "RadialBackground.hpp"
#include "RadialMenu.hpp"
#include "GraphicsCache.hpp"

#include <string>
#include <vector>
#include <map>
#include <memory>

namespace LM {
	class TiledGraphic;
	class IPAddress;
	class Weapon;
	
	class GameController {
	public:
		static const Color		BLUE_COLOR;
		static const Color		RED_COLOR;
		static const Color		BRIGHT_GREEN;
		static const Color		BRIGHT_ORANGE;
		static const Color		BLUE_SHADOW;
		static const Color		RED_SHADOW;
		static const Color		TEXT_COLOR;
		static const Color		TEXT_SHADOW;
		static const Color		GREYED_COLOR;
		static const Color		GREYED_SHADOW;
		static const Color		TEXT_BG_COLOR;
		static const Color		BUTTON_HOVER_COLOR;
		static const Color		BUTTON_HOVER_SHADOW;

		const static int MESSAGE_DISPLAY_TIME;
		const static unsigned int MAX_MESSAGES_TO_DISPLAY;
		const static int SHOT_DISPLAY_TIME;
		const static uint64_t MUZZLE_FLASH_LENGTH;
		const static int GATE_WARNING_FLASH_LENGTH;
		const static double RANDOM_ROTATION_SCALE;
		const static int GATE_STATUS_RECT_WIDTH;
		const static int FROZEN_STATUS_RECT_WIDTH;
		const static int ENERGY_BAR_WIDTH;
		const static int COOLDOWN_BAR_WIDTH;
		const static int STATUS_BAR_HEIGHT;
		const static int DOUBLE_CLICK_TIME;
		const static int NETWORK_TIMEOUT_LIMIT;
		const static int TEXT_LAYER;
		const static unsigned int PING_FREQUENCY;
		const static unsigned int CHAT_TRANSITION_TIME;
		const static unsigned int ROTATION_ADJUST_SPEED;
		
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
			int weapon_1;
			int weapon_2;
			int weapon_3;
		};
		
		enum {
			SHOW_MENUS = 0,
			GAME_IN_PROGRESS = 1,
			GAME_OVER = 2,
			SHOW_OPTIONS_MENU = 3,
			SHOW_SERVER_BROWSER = 4
		};

		struct Message {
			Text*		message;
			Transition*	transition;
			uint64_t	timeout;
		};

		PathManager& 	m_path_manager;
		ClientConfiguration* m_configuration;
	
		GameWindow* 	m_window;
		ClientNetwork	m_network;
		TextManager*	m_text_manager;
		SoundController* m_sound_controller;
		TransitionManager m_transition_manager;
		ServerBrowser*	m_server_browser;
		ChatLog*	m_chat_log;
		Font*		m_font;
		Font*		m_bold_font;
		Font*		m_medium_font;
		Font*		m_menu_font;
		
		std::string	m_name;
		std::string 	m_client_version;
		std::string	m_input_text;
		std::vector<Message> m_messages;
		std::vector<std::pair<Graphic*, unsigned int> > m_shots;
		int		m_protocol_number;
		int 		m_screen_width;
		int 		m_screen_height;
		int		m_map_width;
		int		m_map_height;
		Polygon		m_map_polygon;
		int 		m_pixel_depth;
		int		m_game_state;
		bool 		m_fullscreen;
		bool		m_quit_game;
		bool		m_restart;
		double		m_offset_x;
		double		m_offset_y;
		double		m_mouse_x;
		double		m_mouse_y;
		Uint8*		m_keys;
		KeyBindings	m_key_bindings;
		KeyBindings 	m_alt_key_bindings;
		std::map<uint32_t, GraphicalPlayer> m_players;
		uint32_t 	m_player_id;
		bool		m_holding_gate;
		int		m_gate_lower_sounds[2];
		uint64_t	m_last_clicked;
		uint64_t	m_join_sent_time;
		uint64_t	m_muzzle_flash_start;
		uint64_t	m_last_weapon_switch;
		
		uint64_t	m_last_damage_time;
		uint64_t	m_last_recharge_time;
		uint64_t	m_time_to_unfreeze;
		uint64_t	m_total_time_frozen;
		
		uint64_t	m_round_end_time;
		uint64_t	m_last_ping_sent;
		uint32_t	m_current_ping_id;
		uint64_t	m_ping;
		uint64_t	m_framerate;
		
		// TEMPORARY SPRITE CODE
		GraphicsCache	m_graphics_cache;
		
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
		Transition*	m_chat_window_transition_x;
		Transition*	m_chat_window_transition_y;

		TextMenuItem*	m_version_nag1;
		TextMenuItem*	m_version_nag2;

		Sprite*		m_shot;
	
		Sprite*		m_logo;

		TextMenuItem*	m_item_resume;
		TextMenuItem*	m_item_disconnect;
		ArbitraryMenu	m_main_menu;
		ArbitraryMenu	m_options_menu;
		Form		m_options_form;

		//std::map<std::string, Text*> m_options_menu_items;
		size_t		m_num_resolutions;
		size_t		m_resolution_selected;
		std::vector<std::pair<int, int> > m_resolutions;
		
		bool		m_show_overlay;
		TableBackground* m_overlay_background;
		std::map<std::string, Text*> m_overlay_items;
		ScrollBar* 	m_overlay_scrollbar;
		ScrollArea*	m_overlay_scrollarea;
		
		RadialBackground* m_weapon_selector_back;
		RadialMenu*	m_weapon_selector;
		
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
		
		TableBackground* m_energy_bar;
		TableBackground* m_energy_bar_back;
		Graphic*	m_energy_text;
		
		TableBackground* m_cooldown_bar;
		TableBackground* m_cooldown_bar_back;
		Graphic*	m_curr_weapon_image;
		
		// RADAR CODE BY JEFFREY
		Radar*		m_radar;
		void		set_radar_mode(RadarMode mode); // wrapper around Radar::set_mode
	
		// TEMPORARY MAP CODE BY ANDREW
		GameParameters	m_params;
		GraphicalMap*	m_map;
		std::auto_ptr<MapReceiver>	m_map_receiver;

		// NEW WEAPON CODE
		Weapon*		m_current_weapon;
		std::map<std::string, Weapon*>	m_weapons;

		Weapon*		get_weapon(const std::string& name);
		void		init_weapon_selector();
		void		init_weapons(); //TEMP
		void		set_weapons();
		void		update_curr_weapon_image();
		void		reset_weapons(); // Call reset() on all weapons. Call at end of every round.
		void		clear_weapons(); // Remove all weapons, handling memory management.
	
		// Server Browsing/Scanning
		IPAddress	m_metaserver_address;
		uint32_t	m_current_scan_id;

		void		preinit(ClientConfiguration* config);
		void		init(GameWindow* window);
		void		process_input();
		const GraphicalPlayer* get_player_by_id(uint32_t player_id) const;
		GraphicalPlayer* get_player_by_id(uint32_t player_id);
		GraphicalPlayer* get_player_by_name(const char* name);
		void		send_my_player_update();
		void		attempt_jump();
	
		// Scan a particular server:
		void		scan_server(const IPAddress& server_address);
		void		ping_server(const IPAddress& server_address);
	
		// Scan the local network for servers:
		void		scan_local_network();
	
		// Connect to the meta server to scan the Internet
		void		contact_metaserver();

		bool		load_map(const char* map_name, int map_revision);
		void		request_map();
		void		init_map();

		// Display legalese in the chat window
		void		display_legalese();
	
		void		send_ack(const PacketReader& packet);

		// Misc. helpers

		// Return the current angle (in radians) from the player to the crosshairs
		//  Doesn't take into account the player rotation
		double		get_crosshairs_angle() const;
		
	public:
		explicit GameController(PathManager& pathman, ClientConfiguration* config);
		GameController(PathManager& pathman, ClientConfiguration* config, int width, int height, bool fullscreen =false, int depth =24);
		~GameController();
		
		void		run(int lockfps=60);
		void		set_screen_dimensions(int width, int height);
		void		initialize_key_bindings();
		void		parse_key_input();
		void		move_objects(float timescale);
		void		rotate_towards_angle(double angle_of_incidence, uint64_t duration);
		void		connect_to_server(const IPAddress& server_address, char team =0);
		void		connect_to_server(int servernum);
		void		disconnect();
		void		send_message(std::string message);
		void		send_team_message(std::string message);
		void		display_message(std::string message) { display_message(message, TEXT_COLOR, TEXT_SHADOW); }
		void		display_message(std::string message, Color, Color, bool bold=false);
		void		send_gate_hold(bool holding);
		void		set_gate_hold(bool holding);
		void		set_players_visible(bool visible);
		void		process_mouse_click(SDL_Event event);
		void		send_animation_packet(std::string sprite, std::string field, int value);
		void		send_name_change_packet(const char* new_name);
		void		send_team_change_packet(char new_team);
		void		toggle_main_menu(bool visible);
		void		toggle_options_menu(bool visible);
		void		toggle_score_overlay(bool visible);
		void		toggle_server_browser(bool visible);
		void		set_hud_visible(bool visible);
		void		update_energy_bar(int new_energy=-1);
		void		update_cooldown_bar(double new_cooldown=-1);
		void		delete_server_browser_entry(int num);
		void		change_team_scores(int bluescore, int redscore);
		void 		update_individual_scores();
		void 		update_individual_score_line(int count, const GraphicalPlayer& currplayer);
		void		delete_individual_score(const GraphicalPlayer& currplayer);
		void		set_player_name(std::string name);
		std::string	get_player_name() const { return m_name; };
		void		clear_players();
		bool		wants_restart();
		void		reset_options();
		void		change_weapon(const char* name);
		void		change_weapon(unsigned int n); // change to the nth weapon (0-indexed)
		void		change_weapon(Weapon* weapon);
		void		recreate_name(GraphicalPlayer* player);
		std::string	get_server_address();
		std::string	format_time_from_millis(uint64_t milliseconds);

		void		freeze(uint64_t how_long);

		// This is a COMPATIBILITY WRAPPER around the more general shoot_in_line() function below.
		// Code should be migrated to use the new function.
		Point		find_shootable_object(Point startpos, double direction, BaseMapObject*& hit_map_object, Player*& hit_player);

		// A HitObject represents an object (player or map object) that was hit by a weapon discharge
		struct HitObject {
			double		distance;	// Distance from where the weapon was discharged
			Point		point;		// Where in the arena that the object was hit
			BaseMapObject*	map_object;	// The map object that was hit (if applicable, NULL otherwise)
			Player*		player;		// The player that was hit (if applicable, NULL otherwise)

			HitObject (double distance, Point point, BaseMapObject* map_object =NULL);
			HitObject (double distance, Point point, Player* player);
			bool operator<(const HitObject& other) const { return distance < other.distance; }
		};

		// Starting from the given point, shoot in a STRAIGHT LINE in the given direction,
		// and populate the given set with the objects that are hit.
		void		shoot_in_line(Point startpos, double direction, std::multiset<HitObject>& hit_objects);

		// Find all hit objects in the given shape, and populate the given set
		//void		shoot_in_region(const Shape& shape, bool penetrate_players, bool penetrate_obstacles, std::list<Player*>& hit_players); // TODO (#141)
		
		// Scan both the local network and the meta server for servers:
		void		scan_all();

		// Contact the meta server to check for upgrades
		void		check_for_upgrade();
		
		// Network callbacks:
		void		send_packet(PacketWriter& packet);

		void		welcome(PacketReader& reader);
		void		player_update(PacketReader& reader);
		void		announce(PacketReader& reader);
		void		leave(PacketReader& reader);
		void		weapon_discharged(PacketReader& reader);
		void		player_hit(PacketReader& reader);
		void		message(PacketReader& reader);
		void		gate_update(PacketReader& reader);
		void		new_round(PacketReader& reader);
		void		round_over(PacketReader& reader);
		void		round_start(PacketReader& reader);
		void		spawn_packet(PacketReader& reader);
		void		score_update(PacketReader& reader);
		void		animation_packet(PacketReader& reader);
		void		request_denied(PacketReader& reader);
		void		name_change(PacketReader& reader);
		void		team_change(PacketReader& reader);
		void		map_info_packet(PacketReader& reader);
		void		map_object_packet(PacketReader& reader);
		void		game_param_packet(PacketReader& reader);
		void		player_died(PacketReader& reader);
		void		weapon_select(PacketReader& reader);
		void		weapon_info_packet(PacketReader& reader);
		void		server_info(const IPAddress& server_address, PacketReader& reader);
		void		upgrade_available(const IPAddress& server_address, PacketReader& reader);
		void		hole_punch_packet(const IPAddress& server_address, PacketReader& reader);
	
		// Sound callbacks:
		void		play_sound(const char* name);
		void		sound_finished(int channel);

		// Weapon callbacks:
		void		activate_radar_blip(const Player& player);
		void		show_muzzle_flash();
		void		show_bullet_impact(Point position);

		// Damage the player by this amount of energy
		// aggressor is the player who did the damage, or NULL if no player did it (e.g. hazerdous map object)
		// Returns true if the player died as a result, false if the player is still alive
		bool		damage (int amount, const Player* aggressor);
	};
}

#endif

