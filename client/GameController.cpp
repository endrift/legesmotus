/*
 * GameController.cpp
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
 
#include "GameController.hpp"
#include "GameWindow.hpp"
#include "ClientNetwork.hpp"
#include "GraphicalPlayer.hpp"
#include "Sprite.hpp"
#include "TiledGraphic.hpp"
#include "TableBackground.hpp"
#include "ClientConfiguration.hpp"
#include "ServerBrowser.hpp"
#include "TransitionManager.hpp"
#include "BaseMapObject.hpp"
#include "TextMenuItem.hpp"
#include "common/PacketReader.hpp"
#include "common/PacketWriter.hpp"
#include "common/network.hpp"
#include "common/math.hpp"
#include "common/team.hpp"
#include "common/StringTokenizer.hpp"
#include "common/IPAddress.hpp"
#include "common/timer.hpp"
#include "common/misc.hpp"
#include "common/Shape.hpp"
#include "common/Circle.hpp"

#include "SDL_image.h"

#include <vector>
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <iostream>
#include <limits>
#include <sstream>

using namespace LM;
using namespace std;

const int GameController::MESSAGE_DISPLAY_TIME = 10000;
const unsigned int GameController::MAX_MESSAGES_TO_DISPLAY = 20;
const int GameController::SHOT_DISPLAY_TIME = 180;
const int GameController::MUZZLE_FLASH_LENGTH = 80;
const int GameController::GATE_WARNING_FLASH_LENGTH = 3000;
const double GameController::RANDOM_ROTATION_SCALE = 1.0;
const Color GameController::BLUE_COLOR(0.6, 0.6, 1.0);
const Color GameController::RED_COLOR(1.0, 0.6, 0.6);
const Color GameController::BLUE_SHADOW(0.1, 0.1, 0.3);
const Color GameController::RED_SHADOW(0.3, 0.1, 0.1);
const Color GameController::TEXT_COLOR(1.0, 1.0, 1.0);
const Color GameController::TEXT_SHADOW(0.1, 0.1, 0.1);
const Color GameController::GREYED_COLOR(0.5, 0.5, 0.5);
const Color GameController::GREYED_SHADOW(0.2, 0.2, 0.2);
const Color GameController::TEXT_BG_COLOR(0.0, 0.0, 0.0, 0.7);
const Color GameController::BUTTON_HOVER_COLOR(0.6, 0.7, 0.9);
const Color GameController::BUTTON_HOVER_SHADOW(0.1, 0.2, 0.4);
const int GameController::GATE_STATUS_RECT_WIDTH = 80;
const int GameController::FROZEN_STATUS_RECT_WIDTH = 60;
const int GameController::HEALTH_BAR_WIDTH = 100;
const int GameController::STATUS_BAR_HEIGHT = 40;
const int GameController::DOUBLE_CLICK_TIME = 300;
const int GameController::NETWORK_TIMEOUT_LIMIT = 10000;
const int GameController::TEXT_LAYER = -4;
const unsigned int GameController::PING_FREQUENCY = 2000;
const unsigned int GameController::CHAT_TRANSITION_TIME = 200;
const unsigned int GameController::ROTATION_ADJUST_SPEED = 500;

static bool	sort_resolution(pair<int, int> pairone, pair<int, int> pairtwo) {
	if (pairone.first == pairtwo.first) {
		return pairone.second < pairtwo.second;
	}
	return pairone.first < pairtwo.first;
}

GameController::GameController(PathManager& path_manager, ClientConfiguration* config) : m_path_manager(path_manager) {
	preinit(config);
	init(GameWindow::get_optimal_instance(config->get_int_value("multisample")));
}

GameController::GameController(PathManager& path_manager, ClientConfiguration* config, int width, int height, bool fullscreen, int depth) : m_path_manager(path_manager) {
	preinit(config);
	init(GameWindow::get_instance(width, height, depth, fullscreen, config->get_int_value("multisample")));
}

/*
 * Delete all of the sprites and subsystems.
 */
GameController::~GameController() {
	// TEMPORARY SPRITE CODE
	delete blue_sprite;
	delete blue_front_arm;
	delete blue_back_arm;
	delete red_sprite;
	delete red_front_arm;
	delete red_back_arm;
	delete m_crosshairs;
	delete gun_normal;

	for (unsigned int i = 0; i < m_shots.size(); i++) {
		m_window->unregister_graphic(m_shots[i].first);
		delete m_shots[i].first;
		m_shots.erase(m_shots.begin() + i);
	}

	for (unsigned int i = 0; i < m_messages.size(); i++) {
		m_text_manager->remove_string(m_messages[i].message);
		m_transition_manager.remove_transition(m_messages[i].transition);
		m_messages.erase(m_messages.begin() + i);
	}
	
	m_text_manager->remove_all_strings();
	
	delete m_overlay_background;
	
	delete m_server_browser;
	delete m_chat_log;
	
	delete m_blue_gate_status_rect;
	delete m_blue_gate_status_rect_back;
	delete m_red_gate_status_rect;
	delete m_red_gate_status_rect_back;
	delete m_frozen_status_rect;
	delete m_frozen_status_rect_back;
	delete m_health_bar;
	delete m_health_bar_back;
	
	delete m_input_bar_back;
	delete m_chat_window_back;

	// TEMPORARY MAP CODE BY ANDREW
	delete m_map;

	delete m_text_manager;
	m_sound_controller->destroy_instance();
	delete m_font;
	delete m_menu_font;
	delete m_medium_font;

	delete m_shot;
	delete m_logo;

	m_radar->unregister_with_window(m_window);
	delete m_radar;

	// The GameWindow instance should always be destroyed last, since other stuff may depend on it.
	m_window->deinit_video();
	m_window->destroy_instance();
}

/*
 * Initialize the base of the game controller before the Window gets created
 */
void GameController::preinit(ClientConfiguration* config) {
#ifndef __WIN32
	GameWindow::set_icon(IMG_Load(m_path_manager.data_path("blue_head512.png", "sprites")));
#else
	GameWindow::set_icon(IMG_Load(m_path_manager.data_path("blue_head32.png", "sprites")));
#endif
	m_configuration = config;
}

/*
 * Initialize all of the subsystems, sprites, etc.
 */
void GameController::init(GameWindow* window) {
	srand ( time(NULL) );
	
	get_ticks();
	
	initialize_key_bindings();
	
	// Initial game state will be showing the main menu.
	m_game_state = SHOW_MENUS;
	m_restart = false;
	
	m_screen_width = window->get_width();
	m_screen_height = window->get_height();
	
	m_input_text = "> ";
	m_input_bar = NULL;
	
	m_join_sent_time = 0;
	m_last_ping_sent = 0;
	
	m_client_version = LM_VERSION;
	m_protocol_number = 3;
	
	m_pixel_depth = window->get_depth();
	m_fullscreen = window->is_fullscreen();
	m_quit_game = false;
	m_window = window;

	m_time_to_unfreeze = 0;
	m_total_time_frozen = 0;
	m_last_fired = 0;
	m_last_clicked = 0;

	m_font = new Font(m_path_manager.data_path("JuraMedium.ttf", "fonts"), 14);
	m_text_manager = new TextManager(m_font, m_window);
	
	m_menu_font = new Font(m_path_manager.data_path("JuraDemiBold.ttf", "fonts"), 30);
	m_medium_font = new Font(m_path_manager.data_path("JuraMedium.ttf", "fonts"), 20);
	
	m_sound_controller = SoundController::get_instance(*this, m_path_manager);
	m_holding_gate = false;
	m_gate_lower_sounds[0] = -1;
	m_gate_lower_sounds[1] = -1;
	m_sound_controller->set_sound_on(m_configuration->get_bool_value("sound"));

	m_map = new GraphicalMap(m_path_manager, m_window);
	m_map_width = 0;
	m_map_height = 0;

	// Initialize the gun sprites.
	gun_normal = new Sprite(m_path_manager.data_path("gun_noshot.png", "sprites"));
	gun_normal->set_x(3);
	gun_normal->set_y(19);
	gun_normal->set_rotation(-15);
	gun_normal->set_priority(-1);
	
	m_gun_fired = new Sprite(m_path_manager.data_path("gun_shot.png", "sprites"));
	m_gun_fired->set_x(3);
	m_gun_fired->set_y(19);
	m_gun_fired->set_rotation(-15);
	m_gun_fired->set_priority(-1);
	m_gun_fired->set_invisible(true);
	
	// Initialize all of the components of the player sprites.
	blue_sprite = new Sprite(m_path_manager.data_path("blue_armless.png","sprites"));
	blue_back_arm = new Sprite(m_path_manager.data_path("blue_backarm.png","sprites"));
	blue_front_arm = new Sprite(m_path_manager.data_path("blue_frontarm.png","sprites"));
	blue_back_arm->set_center_x(27);
	blue_back_arm->set_center_y(29);
	blue_back_arm->set_x(-5);
	blue_back_arm->set_y(-20);
	blue_front_arm->set_center_x(49);
	blue_front_arm->set_center_y(33);
	blue_front_arm->set_x(17);
	blue_front_arm->set_y(-16);
	blue_front_arm->set_priority(-2);
	blue_back_arm->set_priority(1);
	
	blue_player.add_graphic(blue_sprite, "torso");
	blue_player.add_graphic(blue_back_arm, "backarm");
	blue_arm_gun.add_graphic(gun_normal, "gun");
	blue_arm_gun.add_graphic(m_gun_fired, "gun_fired");
	blue_arm_gun.add_graphic(blue_front_arm, "arm");
	blue_arm_gun.set_center_x(13);
	blue_arm_gun.set_center_y(-18);
	blue_arm_gun.set_x(13);
	blue_arm_gun.set_y(-18);
	blue_player.add_graphic(&blue_arm_gun, "frontarm");
	blue_player.set_invisible(true);
	
	red_sprite = new Sprite(m_path_manager.data_path("red_armless.png","sprites"));
	red_back_arm = new Sprite(m_path_manager.data_path("red_backarm.png","sprites"));
	red_front_arm = new Sprite(m_path_manager.data_path("red_frontarm.png","sprites"));
	red_front_arm->set_center_x(49);
	red_front_arm->set_center_y(33);
	red_front_arm->set_x(17);
	red_front_arm->set_y(-16);
	red_back_arm->set_center_x(27);
	red_back_arm->set_center_y(29);
	red_back_arm->set_x(-5);
	red_back_arm->set_y(-20);
	red_front_arm->set_priority(-2);
	red_back_arm->set_priority(1);
	
	red_player.add_graphic(red_sprite, "torso");
	red_player.add_graphic(red_back_arm, "backarm");
	red_arm_gun.add_graphic(gun_normal, "gun");
	red_arm_gun.add_graphic(m_gun_fired, "gun_fired");
	red_arm_gun.add_graphic(red_front_arm, "arm");
	red_arm_gun.set_center_x(13);
	red_arm_gun.set_center_y(-18);
	red_arm_gun.set_x(13);
	red_arm_gun.set_y(-18);
	red_player.add_graphic(&red_arm_gun, "frontarm");
	red_player.set_invisible(true);
	
	m_crosshairs = new Sprite(m_path_manager.data_path("crosshairs.png", "sprites"));
	m_crosshairs->set_priority(-5);
	m_window->register_hud_graphic(m_crosshairs);
	
	m_shot = new Sprite(m_path_manager.data_path("shot.png", "sprites"));
	m_shot->set_invisible(true);
	
	m_logo = new Sprite(m_path_manager.data_path("legesmotuslogo.png", "sprites"));
	m_logo->set_x(m_screen_width/2);
	m_logo->set_y(100);
	m_logo->set_priority(0);
	m_window->register_hud_graphic(m_logo);
	
	m_options_menu_items.clear();
	
	// Set the text manager to draw a shadow behind everything.
	ConstantCurve curve(0, 1);
	m_text_manager->set_active_font(m_menu_font);
	m_text_manager->set_shadow_alpha(1.0);
	m_text_manager->set_shadow_offset(1.0, 1.0);
	if (m_configuration->get_bool_value("text_shadow")) {
		m_text_manager->set_shadow_convolve(&curve, 5, 1.0);
	}
	m_text_manager->set_shadow_color(TEXT_SHADOW);
	m_text_manager->set_shadow(true);
	
	// Initialize all of the menu items.
	
	// Main menu
	m_main_menu.add_item(TextMenuItem::with_manager(m_text_manager, "Connect to Server", "connect", 50, 200));
	m_item_resume = TextMenuItem::with_manager(m_text_manager, "Resume Game", "resume", 50, 240, MenuItem::DISABLED);
	m_main_menu.add_item(m_item_resume);
	m_item_disconnect = TextMenuItem::with_manager(m_text_manager, "Disconnect", "disconnect", 50, 280, MenuItem::DISABLED);
	m_main_menu.add_item(m_item_disconnect);
	m_main_menu.add_item(TextMenuItem::with_manager(m_text_manager, "Options", "submenu:options", 50, 320));
	m_main_menu.add_item(TextMenuItem::with_manager(m_text_manager, "Quit", "quit", 50, 360));
	m_main_menu.add_item(TextMenuItem::with_manager(m_text_manager, "Thanks for playing! Please visit", "", 50, 420, MenuItem::STATIC));
	TextMenuItem* thanks2 = TextMenuItem::with_manager(m_text_manager, "http://legesmotus.cs.brown.edu", "", 50, 460, MenuItem::STATIC);
	thanks2->set_plain_fg_color(Color(0.4, 1.0, 0.4));
	m_main_menu.add_item(thanks2);
	m_main_menu.add_item(TextMenuItem::with_manager(m_text_manager, "to leave feedback for us!", "", 50, 500, MenuItem::STATIC));

	m_text_manager->set_active_font(m_font);
	m_main_menu.add_item(TextMenuItem::with_manager(m_text_manager, string("v. ").append(m_client_version), "", m_screen_width - 90, m_screen_height - 40, MenuItem::STATIC));
	m_text_manager->set_active_font(m_menu_font);

	m_window->register_hud_graphic(m_main_menu.get_graphic_group());

	// Options menu
	m_text_manager->set_active_font(m_menu_font);
	m_options_menu_items["Back"] = m_text_manager->place_string("Cancel", 50, m_screen_height-50, TextManager::LEFT, TextManager::LAYER_HUD);
	m_options_menu_items["Enter Name"] = m_text_manager->place_string("Enter Name", 50, 200, TextManager::LEFT, TextManager::LAYER_HUD);
	if (m_sound_controller->is_sound_on()) {
		m_options_menu_items["Toggle Sound"] = m_text_manager->place_string("Sound: On", 50, 240, TextManager::LEFT, TextManager::LAYER_HUD);
	} else {
		m_options_menu_items["Toggle Sound"] = m_text_manager->place_string("Sound: Off", 50, 240, TextManager::LEFT, TextManager::LAYER_HUD);
	}
	m_options_menu_items["Resolution"] = m_text_manager->place_string("Screen Resolution: ", 50, 280, TextManager::LEFT, TextManager::LAYER_HUD);
	string fullscreen = "";
	if (m_configuration->get_bool_value("fullscreen")) {
		fullscreen = "Fullscreen: Yes";
	} else {
		fullscreen = "Fullscreen: No";
	}
	m_options_menu_items["Fullscreen"] = m_text_manager->place_string(fullscreen, 50, 320, TextManager::LEFT, TextManager::LAYER_HUD);
	m_options_menu_items["Apply"] = m_text_manager->place_string("Apply", m_screen_width - 200, m_screen_height-50, TextManager::LEFT, TextManager::LAYER_HUD);

	// TODO: move this to preinit--it doesn't require a GameWindow, and should be done before one is made
	int depth;
	m_window->supported_resolutions(NULL, NULL, &depth, &m_num_resolutions);
	int supported_widths[m_num_resolutions];
	int supported_heights[m_num_resolutions];
	bool found_res = false;
	m_window->supported_resolutions(supported_widths, supported_heights, &depth, &m_num_resolutions);
	for (size_t i = 0; i < m_num_resolutions; i++) {
		if (m_screen_width == supported_widths[i] && m_screen_height == supported_heights[i]) {
			found_res = true;
		}
		if (supported_widths[i] < 640 || supported_heights[i] < 480) {
			continue;
		}
		m_resolutions.push_back(make_pair(supported_widths[i], supported_heights[i]));
	}
	m_num_resolutions = m_resolutions.size();
	if (found_res == false) {
		m_resolutions.push_back(make_pair(m_screen_width, m_screen_height));
		m_num_resolutions++;
	}
	sort(m_resolutions.begin(), m_resolutions.end(), sort_resolution);
	
	for (size_t i = 0; i < m_num_resolutions; i++) {
		int width = m_resolutions[i].first;
		int height = m_resolutions[i].second;
		stringstream resolution;
		resolution << width << "x" << height;
		if (m_screen_width == width && m_screen_height == height) {
			m_resolution_selected = i;
			m_options_menu_items["CurrResolution"] = m_text_manager->place_string(resolution.str(), 410, 280, TextManager::LEFT, TextManager::LAYER_HUD);
		}
	}
	
	// Server browser
	m_server_browser = new ServerBrowser(*this, m_window, m_text_manager, m_screen_width, m_screen_height, m_font, m_medium_font, m_menu_font);
	m_server_browser->set_visible(false);
	
	// Chat log
	m_chat_log = new ChatLog(*this, m_window, m_text_manager, m_screen_width, m_screen_height, m_font, m_medium_font, m_menu_font);
	m_chat_log->set_visible(false);
	
	// Initialize the overlay.
	m_overlay_background = new TableBackground(3, m_screen_width - 300);
	m_overlay_background->set_row_height(0, 80);
	m_overlay_background->set_row_height(1, 43);
	m_overlay_background->set_row_height(2, m_screen_height - 323);
	m_overlay_background->set_priority(-2);
	m_overlay_background->set_border_color(Color(1,1,1,0.8));
	m_overlay_background->set_border_width(2);
	m_overlay_background->set_cell_color(0, Color(0.1,0.1,0.1,0.8));
	m_overlay_background->set_cell_color(1, Color(0.2,0.1,0.1,0.8));
	m_overlay_background->set_cell_color(2, Color(0.1,0.1,0.15,0.8));
	m_overlay_background->set_y(100);
	m_overlay_background->set_x(m_screen_width/2);
	m_overlay_background->set_border_collapse(true);
	m_overlay_background->set_corner_radius(20);
	m_window->register_hud_graphic(m_overlay_background);
	
	m_overlay_scrollbar = new ScrollBar();
	m_overlay_scrollbar->set_priority(-3);
	m_overlay_scrollbar->set_height(m_overlay_background->get_row_height(2) - 20);
	m_overlay_scrollbar->set_x(m_overlay_background->get_x() + m_overlay_background->get_image_width()/2 - 20);
	m_overlay_scrollbar->set_y(m_overlay_background->get_y() + m_overlay_background->get_row_height(0) + m_overlay_background->get_row_height(1) + 5 + m_overlay_scrollbar->get_height()/2);
	m_overlay_scrollbar->set_section_color(ScrollBar::BUTTONS, Color(0.7,0.2,0.1));
	m_overlay_scrollbar->set_section_color(ScrollBar::TRACK, Color(0.2,0.1,0.1));
	m_overlay_scrollbar->set_section_color(ScrollBar::TRACKER, Color(0.2,0.2,0.4));
	m_overlay_scrollbar->set_scroll_speed(3);

	m_overlay_scrollarea = new ScrollArea(m_overlay_background->get_image_width(),m_overlay_background->get_row_height(2) - 30,10,m_overlay_scrollbar);
	m_overlay_scrollarea->set_priority(TEXT_LAYER);
	m_overlay_scrollarea->get_group()->set_priority(TEXT_LAYER);
	m_overlay_scrollarea->set_x(m_overlay_background->get_x() + 5);
	m_overlay_scrollarea->set_y(m_overlay_background->get_y() + m_overlay_background->get_row_height(0) + m_overlay_background->get_row_height(1) + 15);
	m_overlay_scrollarea->set_center_x(m_overlay_scrollarea->get_width()/2);
	m_overlay_scrollarea->set_center_y(0);

	m_window->register_hud_graphic(m_overlay_scrollbar);
	m_window->register_hud_graphic(m_overlay_scrollarea);
	
	m_overlay_items["red label"] = m_text_manager->place_string("Red Team:", m_overlay_background->get_x() - m_overlay_background->get_image_width()/2 + 10, 115, TextManager::LEFT, TextManager::LAYER_HUD, TEXT_LAYER);
	m_overlay_items["blue label"] = m_text_manager->place_string("Blue Team:", m_overlay_background->get_x(), 115, TextManager::LEFT, TextManager::LAYER_HUD, TEXT_LAYER);
	
	m_text_manager->set_active_font(m_medium_font);
	m_overlay_items["name label"] = m_text_manager->place_string("Name", m_overlay_background->get_x() - m_overlay_background->get_image_width()/2 + 10, 190, TextManager::LEFT, TextManager::LAYER_HUD, TEXT_LAYER);
	m_overlay_items["score label"] = m_text_manager->place_string("Score", m_overlay_background->get_x(), 190, TextManager::LEFT, TextManager::LAYER_HUD, TEXT_LAYER);
	
	change_team_scores(0, 0);
	update_individual_scores();
	
	// Initialize the gate warning.
	m_text_manager->set_active_font(m_menu_font);
	m_text_manager->set_active_color(1.0, 0.4, 0.4);
	m_gate_warning = m_text_manager->place_string("Your gate is going down!", m_screen_width/2, m_screen_height - 200, TextManager::CENTER, TextManager::LAYER_HUD);
	m_gate_warning->set_invisible(true);
	m_gate_warning_time = 0;
	
	// Initialize the gate status bars.
	m_blue_gate_status_rect = new TableBackground(1, GATE_STATUS_RECT_WIDTH);
	m_blue_gate_status_rect->set_row_height(0, STATUS_BAR_HEIGHT);
	m_blue_gate_status_rect->set_priority(-1);
	m_blue_gate_status_rect->set_cell_color(0, Color(0.0, 0.0, 1.0, 0.5));
	m_blue_gate_status_rect->set_x(m_screen_width - 2 * m_blue_gate_status_rect->get_image_width() - 20);
	m_blue_gate_status_rect->set_y(m_screen_height - m_blue_gate_status_rect->get_image_height() - 20);
	m_window->register_hud_graphic(m_blue_gate_status_rect);
	m_blue_gate_status_rect_back = new TableBackground(1, GATE_STATUS_RECT_WIDTH);
	m_blue_gate_status_rect_back->set_row_height(0, STATUS_BAR_HEIGHT);
	m_blue_gate_status_rect_back->set_priority(0);
	m_blue_gate_status_rect_back->set_cell_color(0, Color(0.1, 0.1, 0.1, 0.5));
	m_blue_gate_status_rect_back->set_x(m_screen_width - 2 * m_blue_gate_status_rect->get_image_width() - 20);
	m_blue_gate_status_rect_back->set_y(m_screen_height - m_blue_gate_status_rect->get_image_height() - 20);
	m_window->register_hud_graphic(m_blue_gate_status_rect_back);
	
	m_red_gate_status_rect = new TableBackground(1, GATE_STATUS_RECT_WIDTH);
	m_red_gate_status_rect->set_row_height(0, STATUS_BAR_HEIGHT);
	m_red_gate_status_rect->set_priority(-1);
	m_red_gate_status_rect->set_cell_color(0, Color(1.0, 0.0, 0.0, 0.5));
	m_red_gate_status_rect->set_x(m_screen_width - m_red_gate_status_rect->get_image_width() - 10);
	m_red_gate_status_rect->set_y(m_screen_height - m_red_gate_status_rect->get_image_height() - 20);
	m_window->register_hud_graphic(m_red_gate_status_rect);
	m_red_gate_status_rect_back = new TableBackground(1, GATE_STATUS_RECT_WIDTH);
	m_red_gate_status_rect_back->set_row_height(0, STATUS_BAR_HEIGHT);
	m_red_gate_status_rect_back->set_priority(0);
	m_red_gate_status_rect_back->set_cell_color(0, Color(0.1, 0.1, 0.1, 0.5));
	m_red_gate_status_rect_back->set_x(m_screen_width - m_red_gate_status_rect->get_image_width() - 10);
	m_red_gate_status_rect_back->set_y(m_screen_height - m_red_gate_status_rect->get_image_height() - 20);
	m_window->register_hud_graphic(m_red_gate_status_rect_back);
	
	// Initialize the gate status bar labels.
	m_text_manager->set_active_color(TEXT_COLOR);
	m_text_manager->set_active_font(m_font);
	m_blue_gate_status_text = m_text_manager->place_string("Blue Gate", m_blue_gate_status_rect->get_x() + 1, m_blue_gate_status_rect->get_y() + m_blue_gate_status_rect->get_image_height()/4, TextManager::CENTER, TextManager::LAYER_HUD, TEXT_LAYER);
	m_red_gate_status_text = m_text_manager->place_string("Red Gate", m_red_gate_status_rect->get_x() + 2, m_red_gate_status_rect->get_y() + m_red_gate_status_rect->get_image_height()/4, TextManager::CENTER, TextManager::LAYER_HUD, TEXT_LAYER);
	
	// Initialize the frozen status bar.
	m_frozen_status_rect = new TableBackground(1, FROZEN_STATUS_RECT_WIDTH);
	m_frozen_status_rect->set_row_height(0, 20);
	m_frozen_status_rect->set_priority(-1);
	m_frozen_status_rect->set_cell_color(0, Color(0.0, 0.5, 1.0, 0.5));
	m_frozen_status_rect->set_x(m_screen_width/2);
	m_frozen_status_rect->set_y(m_screen_height/2 + 50);
	m_window->register_hud_graphic(m_frozen_status_rect);
	m_frozen_status_rect_back = new TableBackground(1, FROZEN_STATUS_RECT_WIDTH);
	m_frozen_status_rect_back->set_row_height(0, 20);
	m_frozen_status_rect_back->set_priority(0);
	m_frozen_status_rect_back->set_cell_color(0, Color(0.1, 0.1, 0.1, 0.5));
	m_frozen_status_rect_back->set_x(m_frozen_status_rect->get_x());
	m_frozen_status_rect_back->set_y(m_frozen_status_rect->get_y());
	m_window->register_hud_graphic(m_frozen_status_rect_back);
	
	// Initialize the frozen status bar label.
	m_text_manager->set_active_color(1.0, 1.0, 1.0);
	m_text_manager->set_active_font(m_font);
	m_frozen_status_text = m_text_manager->place_string("Frozen", m_frozen_status_rect->get_x() + 1, m_frozen_status_rect->get_y() + 2, TextManager::CENTER, TextManager::LAYER_HUD, TEXT_LAYER);

	// Initialize the frozen status bar.
	m_health_text = NULL;
	m_health_bar = new TableBackground(1, HEALTH_BAR_WIDTH);
	m_health_bar->set_row_height(0, STATUS_BAR_HEIGHT);
	m_health_bar->set_priority(-1);
	m_health_bar->set_cell_color(0, Color(1.0, 0.0, 0.0, 0.5));
	m_health_bar->set_x(HEALTH_BAR_WIDTH);
	m_health_bar->set_y(m_screen_height - m_health_bar->get_image_height() - 20);
	m_window->register_hud_graphic(m_health_bar);
	m_health_bar_back = new TableBackground(1, HEALTH_BAR_WIDTH);
	m_health_bar_back->set_row_height(0, STATUS_BAR_HEIGHT);
	m_health_bar_back->set_priority(0);
	m_health_bar_back->set_cell_color(0, Color(0.1, 0.1, 0.1, 0.5));
	m_health_bar_back->set_x(m_health_bar->get_x());
	m_health_bar_back->set_y(m_health_bar->get_y());
	m_window->register_hud_graphic(m_health_bar_back);
	
	update_health_bar(100);

	// Initialize the input bar background
	m_input_bar_back = new TableBackground(1, 0);
	m_input_bar_back->set_row_height(0, 20);
	m_input_bar_back->set_priority(0);
	m_input_bar_back->set_cell_color(0, TEXT_BG_COLOR);
	m_input_bar_back->set_x(17);
	m_input_bar_back->set_y(m_screen_height - 102);
	m_input_bar_back->set_invisible(true);
	m_window->register_hud_graphic(m_input_bar_back);
	
	// Initialize the chat window background
	m_chat_window_back = new TableBackground(1, 0);
	m_chat_window_back->set_row_height(0, 20);
	m_chat_window_back->set_priority(0);
	m_chat_window_back->set_cell_color(0, TEXT_BG_COLOR);
	m_chat_window_back->set_x(17);
	m_chat_window_back->set_y(17);
	m_chat_window_back->set_invisible(true);
	m_window->register_hud_graphic(m_chat_window_back);

	m_chat_window_transition_x = new Transition(m_chat_window_back, &Graphic::set_width, new LinearCurve(0,0));
	m_chat_window_transition_x->set_curve_ownership(true);
	m_transition_manager.add_transition(m_chat_window_transition_x);

	m_chat_window_transition_y = new Transition(m_chat_window_back, &Graphic::set_height, new LinearCurve(0,0));
	m_chat_window_transition_y->set_curve_ownership(true);
	m_transition_manager.add_transition(m_chat_window_transition_y);
	
	// Set up the radar.
	m_radar = new Radar(m_path_manager, m_params.radar_scale, m_params.radar_mode);
	m_radar->set_x(m_screen_width - 120);
	m_radar->set_y(120);
	m_radar->register_with_window(m_window);

	m_current_scan_id = 0;
	// TODO: better error messages if meta server address can't be resolved
	if (const char* metaserver_address = getenv("LM_METASERVER")) {
		// Address specified by $LM_METASERVER environment avariable
		if (!resolve_hostname(m_metaserver_address, metaserver_address)) {
			cerr << "Unable to resolve metaserver hostname, `" << metaserver_address << "' as specified in the $LM_METASERVER environment variable.  Internet-wide server browsing will not be enabled." << endl;
		}
	} else if (!resolve_hostname(m_metaserver_address, METASERVER_HOSTNAME, METASERVER_PORTNO)) {
		cerr << "Unable to resolve metaserver hostname.  Internet-wide server browsing will not be enabled." << endl;
	}

	check_for_upgrade();
}

/*
 * The main game loop.
 */
void GameController::run(int lockfps) {
	cout << "SDL window is: " << m_window->get_width() << " pixels wide and " 
		<< m_window->get_height() << " pixels tall." << endl;
	
	unsigned long startframe = get_ticks();
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
		
		if (m_join_sent_time != 0 && m_join_sent_time + 5000 < get_ticks()) {
			display_message("Error: Could not connect to server.", RED_COLOR, RED_SHADOW);
			disconnect();
			m_join_sent_time = 0;
		}
		
		// Check if my player is set to unfreeze.
		if (!m_players.empty() && m_time_to_unfreeze < get_ticks() && m_time_to_unfreeze != 0) {
			m_sound_controller->play_sound("unfreeze");
			m_players[m_player_id].reset_health();
			update_health_bar();
			m_players[m_player_id].set_is_frozen(false);
			if (m_radar->get_mode() == RADAR_ON) {
				m_radar->set_blip_alpha(m_player_id, 1.0);
			}
			m_time_to_unfreeze = 0;
			m_total_time_frozen = 0;
		}
		
		// Update movement twice as often as graphics.
		unsigned long currframe = get_ticks();
		if ((currframe - lastmoveframe) >= (delay/2)) {
			move_objects((get_ticks() - lastmoveframe) / delay); // scale all position changes to keep game speed constant. 
			
			if (m_time_to_unfreeze != 0) {
				m_frozen_status_rect->set_image_width(((m_time_to_unfreeze - get_ticks())/(double)m_total_time_frozen) * FROZEN_STATUS_RECT_WIDTH);
			}
			
			lastmoveframe = get_ticks();
		}
		
		
		// Update graphics if frame rate is correct.
		if((currframe - startframe) >= delay) {
			if (m_network.is_connected() && m_join_sent_time == 0) {
				if ((currframe - m_last_ping_sent) >= PING_FREQUENCY) {
					ping_server(m_network.get_server_address());
					m_last_ping_sent = currframe;
				}
				
				if (m_network.get_last_packet_time() + NETWORK_TIMEOUT_LIMIT < currframe) {
					display_message("Connection to the server has timed out.", RED_COLOR, RED_SHADOW);
					disconnect();
					continue;
				}
			}
		
			if (!m_server_browser->is_invisible()) {
				m_server_browser->autoscroll(currframe - startframe);
			}
			if (!m_overlay_scrollbar->is_invisible()) {
				m_overlay_scrollbar->autoscroll(currframe - startframe);
			}
			if (!m_chat_log->is_invisible()) {
				m_chat_log->autoscroll(currframe - startframe);
			}
		
			bool erasedone = false;
			// Erase messages that are too old.
			double height = m_chat_window_transition_y->get_curve()->get_end();
			for (vector<Message>::iterator iter = m_messages.begin();
					iter != m_messages.end();) {
				if (iter->timeout < currframe || m_messages.size() > MAX_MESSAGES_TO_DISPLAY) {
					height -= iter->message->get_image_height();
					m_text_manager->remove_string(iter->message);
					m_transition_manager.remove_transition(iter->transition);
					delete iter->transition;
					iter = m_messages.erase(iter);
					erasedone = true;
				} else {
				 iter++;
				}
			}

			
			if (erasedone) {
				m_chat_window_transition_y->change_curve(currframe, new LinearCurve(0, height), CHAT_TRANSITION_TIME);
				m_chat_window_back->set_image_width(0);
				// Reposition messages that remain after removing.
				double max_w = 0;
				for (unsigned int i = 0; i < m_messages.size(); i++) {
					int y = 20 + (m_font->ascent() + m_font->descent() + 5) * i;
					m_messages[i].transition->change_curve(currframe, new LinearCurve(0, y), CHAT_TRANSITION_TIME);
					max_w = max<double>(m_messages[i].message->get_image_width() + 6, max_w);
				}
				m_chat_window_transition_x->change_curve(currframe, new LinearCurve(0, max_w), CHAT_TRANSITION_TIME);
				if (m_messages.size() == 0) {
					m_chat_window_back->set_invisible(true);
				}
			}
			
			// Change shot displays based on time.
			for (unsigned int i = 0; i < m_shots.size(); i++) {
				double shot_time = (double)(SHOT_DISPLAY_TIME-(m_shots[i].second - currframe))/SHOT_DISPLAY_TIME;
				double shot_curve = -4.5*shot_time*(shot_time-1.0)/(shot_time+0.5); //fancy curve
				m_shots[i].first->set_scale_x(shot_curve);
				m_shots[i].first->set_scale_y(shot_curve);
				m_shots[i].first->set_rotation(shot_time*90.0);
				if (m_shots[i].second < currframe) {
					m_window->unregister_graphic(m_shots[i].first);
					delete m_shots[i].first;
					m_shots.erase(m_shots.begin() + i);
				}
			}
			
			// Change the display of the gate warning message based on time.
			if (m_gate_warning_time != 0 && m_gate_warning_time < currframe - GATE_WARNING_FLASH_LENGTH) {
				m_gate_warning_time = 0;
				m_gate_warning->set_invisible(true);
			} else if (m_gate_warning_time != 0 && m_gate_warning_time < currframe - (5*GATE_WARNING_FLASH_LENGTH)/6) {
				m_gate_warning->set_invisible(false);
			} else if (m_gate_warning_time != 0 && m_gate_warning_time < currframe - (4*GATE_WARNING_FLASH_LENGTH)/6) {
				m_gate_warning->set_invisible(true);
			} else if (m_gate_warning_time != 0 && m_gate_warning_time < currframe - (3*GATE_WARNING_FLASH_LENGTH)/6) {
				m_gate_warning->set_invisible(false);
			} else if (m_gate_warning_time != 0 && m_gate_warning_time < currframe - (2*GATE_WARNING_FLASH_LENGTH)/6) {
				m_gate_warning->set_invisible(true);
			} else if (m_gate_warning_time != 0 && m_gate_warning_time < currframe - (1*GATE_WARNING_FLASH_LENGTH)/6) {
				m_gate_warning->set_invisible(false);
			}
			
			// Uncomment if framerate is needed.
			// the framerate:
			m_framerate = (1000/(currframe - startframe));
			m_transition_manager.update(currframe);
			
			if (!m_players.empty()) {
				//cerr << m_players[m_player_id].get_sprite()->get_rotation() << ", ";
				m_players[m_player_id].set_rotation_degrees(m_players[m_player_id].get_sprite()->get_rotation());
								
				// Change gun sprite if muzzle flash is done.
				Graphic* frontarm = m_players[m_player_id].get_sprite()->get_graphic("frontarm");
				if (m_last_fired < get_ticks() - MUZZLE_FLASH_LENGTH && frontarm->get_graphic("gun")->is_invisible()) {
					frontarm->get_graphic("gun")->set_invisible(false);
					send_animation_packet("frontarm/gun", "invisible", false);
					frontarm->get_graphic("gun_fired")->set_invisible(true);
					send_animation_packet("frontarm/gun_fired", "invisible", true);
				}
							
				m_crosshairs->set_x(m_mouse_x);
				m_crosshairs->set_y(m_mouse_y);
				
				// Turn arm of player to face crosshairs.
				if (!m_players[m_player_id].is_frozen()) {
					double	angle = get_normalized_angle(to_degrees(get_crosshairs_angle()) - m_players[m_player_id].get_rotation_degrees());
					
					if (angle < 90 || angle > 270) {
						m_players[m_player_id].get_sprite()->set_scale_x(-1);
						send_animation_packet("all", "scale_x", -1);
						angle *= -1;
						angle += 55;
					} else {
						m_players[m_player_id].get_sprite()->set_scale_x(1);
						send_animation_packet("all", "scale_x", 1);
						angle -= 120;
					}
					m_players[m_player_id].get_sprite()->get_graphic("frontarm")->set_rotation(angle);
					send_animation_packet("frontarm", "rotation", int(round(angle))); // XXX: going double->int here
				}
				send_my_player_update();
				m_radar->update(currframe);

				// Set the new offset of the window so that the player is centered.
				m_offset_x = m_players[m_player_id].get_x() - (m_screen_width/2.0);
				m_offset_y = m_players[m_player_id].get_y() - (m_screen_height/2.0);
				m_window->set_offset_x(m_offset_x);
				m_window->set_offset_y(m_offset_y);
			}
			
			// Show and hide elements depending on game state (started, menus, etc.)
			if (m_game_state == SHOW_MENUS) {
				if (m_map != NULL) {
					m_map->set_visible(false);
				}
				
				set_players_visible(false);
				
				for ( unsigned int i = 0; i < m_shots.size(); i++ ) {
					m_shots[i].first->set_invisible(true);
				}
				
				set_hud_visible(false);
				
				m_logo->set_invisible(false);
				
				toggle_main_menu(true);
				toggle_options_menu(false);
				m_server_browser->set_visible(false);
			} else if (m_game_state == SHOW_OPTIONS_MENU) {
				if (m_map != NULL) {
					m_map->set_visible(false);
				}
				
				set_players_visible(false);
				
				for ( unsigned int i = 0; i < m_shots.size(); i++ ) {
					m_shots[i].first->set_invisible(true);
				}
				
				set_hud_visible(false);
				
				m_logo->set_invisible(false);
				
				toggle_main_menu(false);
				toggle_options_menu(true);
				m_server_browser->set_visible(false);
			} else if (m_game_state == SHOW_SERVER_BROWSER) {
				if (m_map != NULL) {
					m_map->set_visible(false);
				}
				
				set_players_visible(false);
				
				for ( unsigned int i = 0; i < m_shots.size(); i++ ) {
					m_shots[i].first->set_invisible(true);
				}
				
				set_hud_visible(false);
				
				m_logo->set_invisible(false);
				
				toggle_main_menu(false);
				toggle_options_menu(false);
				m_server_browser->set_visible(true);
			} else {
				if (m_map != NULL) {
					m_map->set_visible(true);
				}
				
				set_players_visible(true);
				
				for ( unsigned int i = 0; i < m_shots.size(); i++ ) {
					m_shots[i].first->set_invisible(false);
				}
				
				set_hud_visible(true);
				
				m_logo->set_invisible(true);
				
				toggle_main_menu(false);
				toggle_options_menu(false);
				m_server_browser->set_visible(false);
			}
			
			m_window->redraw();
			startframe = get_ticks();
		}
	}
	
	if (!m_restart) {
		disconnect();
	}
}

/*
 * Set the HUD visible or invisible.
 */
void GameController::set_hud_visible(bool visible) {
	m_radar->set_invisible(!visible);
	
	m_blue_gate_status_rect->set_invisible(!visible);
	m_blue_gate_status_text->set_invisible(!visible);
	m_blue_gate_status_rect_back->set_invisible(!visible);
	m_red_gate_status_rect->set_invisible(!visible);
	m_red_gate_status_text->set_invisible(!visible);
	m_red_gate_status_rect_back->set_invisible(!visible);
	
	GraphicalPlayer* player = get_player_by_id(m_player_id);
	
	if (player == NULL) {
		m_frozen_status_rect->set_invisible(true);
		m_frozen_status_text->set_invisible(true);
		m_frozen_status_rect_back->set_invisible(true);
		m_health_bar->set_invisible(true);
		m_health_bar_back->set_invisible(true);
		m_health_text->set_invisible(true);
		return;
	}
	
	m_health_bar->set_invisible(!visible);
	m_health_bar_back->set_invisible(!visible);
	m_health_text->set_invisible(!visible);
	
	if (player->is_frozen() && !player->is_invisible()) {
		m_frozen_status_rect->set_invisible(!visible);
		m_frozen_status_text->set_invisible(!visible);
		m_frozen_status_rect_back->set_invisible(!visible);
	} else {
		m_frozen_status_rect->set_invisible(true);
		m_frozen_status_text->set_invisible(true);
		m_frozen_status_rect_back->set_invisible(true);
	}
}

/*
 * Update the health bar.
 */
void GameController::update_health_bar(int new_health) {
	if (GraphicalPlayer* player = get_player_by_id(m_player_id)) {
		if (new_health != -1) {
			player->set_health(new_health);
		} else {
			new_health = player->get_health();
		}
	} else {
		new_health = 100;
	}
	
	m_health_bar->set_image_width(HEALTH_BAR_WIDTH * ((double)new_health/GraphicalPlayer::MAX_HEALTH));
	
	if (m_health_text != NULL) {
		m_text_manager->remove_string(m_health_text);
	}
	
	// Re-initialize the label.
	m_text_manager->set_active_color(1.0, 1.0, 1.0);
	m_text_manager->set_active_font(m_font);
	
	ostringstream healthstring;
	healthstring << "H: " << new_health;
	m_health_text = m_text_manager->place_string(healthstring.str(), m_health_bar->get_x(), m_health_bar->get_y() + STATUS_BAR_HEIGHT/2, TextManager::CENTER, TextManager::LAYER_HUD, TEXT_LAYER);
	m_health_text->set_y(m_health_bar->get_y() + STATUS_BAR_HEIGHT/2 - m_health_text->get_image_height()/2);
}

/*
 * Set the dimensions of the screen.
 */
void GameController::set_screen_dimensions(int width, int height) {
	m_screen_width = width;
	m_screen_height = height;
}

/*
 * Process the current SDL input state.
 */
void GameController::process_input() {
	SDL_Event event;
	while(SDL_PollEvent(&event) != 0) {
		switch(event.type) {
			case SDL_KEYDOWN:
				if (event.key.keysym.sym == m_key_bindings.quit || event.key.keysym.sym == m_alt_key_bindings.quit) {
					cerr << "Quit key pressed - quitting." << endl;
					m_quit_game = true;
				}
				
				// If we're typing into the input bar...
				if (m_input_bar != NULL) {
					if (event.key.keysym.sym == m_key_bindings.show_overlay || event.key.keysym.sym == m_alt_key_bindings.show_overlay) {
						parse_key_input();
						break;
					}
				
					m_text_manager->set_active_color(TEXT_COLOR);
					// If we're going back to the menu, remove the input bar.
					if (event.key.keysym.sym == m_key_bindings.show_menu || event.key.keysym.sym == m_alt_key_bindings.show_menu) {
						if (!m_chat_log->is_invisible()) {
							m_chat_log->set_visible(false);
						}
						SDL_EnableUNICODE(0);
						SDL_EnableKeyRepeat(0, 0); // Disable key repeat
						m_text_manager->remove_string(m_input_bar);
						m_input_bar = NULL;
						m_input_bar_back->set_invisible(true);
						m_input_text = "> ";
					} else if (event.key.keysym.sym == m_key_bindings.send_chat || event.key.keysym.sym == m_alt_key_bindings.send_chat) {
						if (m_input_text.find("[TEAM]> ") == 0) {
							string msg = m_input_text.substr(8);
							m_input_text = ((string)"> /tchat ").append(msg);
						}
						
						// Remove the "> " from the front.
						string message = m_input_text.substr(2);
						
						// Check message for commands.
						if (message == "/quit") {
							m_quit_game = true;
						} else if (message.find("/name ") == 0) {
							string new_name(message.substr(6));
							if (m_network.is_connected()) {
								send_name_change_packet(new_name.c_str());
							} else {
								ostringstream	msg;
								msg << "You are now known as " << new_name;
								display_message(msg.str());
								set_player_name(new_name);
							}
						} else if (message.find("/team ") == 0) {
							string	new_team_string(message.substr(6));
							char	new_team = parse_team_string(new_team_string.c_str());
							if (is_valid_team(new_team)) {
								send_team_change_packet(new_team);
							} else {
								display_message("Please enter '/team blue' or '/team red'");
							}
						} else if (message.find("/tchat ") == 0) {
							send_team_message(message.substr(7));
						} else if (message == "/copying" || message == "/warranty" || message == "/legal" || message == "/copyright") {
							display_legalese();
						} else {
							send_message(message);
						}
					
						// Remove the input bar.
						SDL_EnableUNICODE(0);
						SDL_EnableKeyRepeat(0, 0); // Disable key repeat
						m_text_manager->remove_string(m_input_bar);
						m_input_bar = NULL;
						m_input_bar_back->set_invisible(true);
						m_input_text = "> ";
					} else if (event.key.keysym.sym == SDLK_BACKSPACE) {
						// Delete text.
						if (m_input_text.length() <= 2) {
							break;
						}
						if (m_input_text.find("[TEAM]> ") == 0 && m_input_text.length() <= 8) {
							break;
						}
						m_input_text.erase(m_input_text.length() - 1);
						m_text_manager->remove_string(m_input_bar);
						m_text_manager->set_active_color(TEXT_COLOR);
						m_text_manager->set_active_font(m_font);
						m_input_bar = m_text_manager->place_string(m_input_text, 20, m_screen_height-100, TextManager::LEFT, TextManager::LAYER_HUD);
						m_input_bar_back->set_image_width(m_input_bar->get_image_width() + 6);
						if (m_configuration->get_bool_value("text_background")) {
							m_input_bar_back->set_invisible(false);
						}
					} else {
						// Otherwise, it's a regular character. Type it in.
						if ( (event.key.keysym.unicode & 0xFF80) == 0 && event.key.keysym.unicode != 0) {
							m_input_text.push_back(event.key.keysym.unicode & 0x7F);
						} else {
							// INTERNATIONAL CHARACTER... DO SOMETHING.
						}
						// Replace the text display with the new one.
						m_text_manager->remove_string(m_input_bar);
						m_text_manager->set_active_color(TEXT_COLOR);
						m_text_manager->set_active_font(m_font);
						m_input_bar = m_text_manager->place_string(m_input_text, 20, m_screen_height-100, TextManager::LEFT, TextManager::LAYER_HUD);
						m_input_bar_back->set_image_width(m_input_bar->get_image_width() + 6);
						if (m_configuration->get_bool_value("text_background")) {
							m_input_bar_back->set_invisible(false);
						}
					}
				} else {
					//Check which key using: event.key.keysym.sym == SDLK_<SOMETHING>
					if (event.key.keysym.sym == m_key_bindings.jump || event.key.keysym.sym == m_alt_key_bindings.jump) {
						attempt_jump();
					} else if (event.key.keysym.sym == m_key_bindings.open_chat || event.key.keysym.sym == m_key_bindings.open_console || event.key.keysym.sym == m_alt_key_bindings.open_chat || event.key.keysym.sym == m_alt_key_bindings.open_console) {
						SDL_EnableUNICODE(1);
						SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);
						m_text_manager->set_active_color(TEXT_COLOR);
						if (m_input_bar == NULL) {
							m_text_manager->set_active_font(m_font);
							m_input_bar = m_text_manager->place_string("> ", 20, m_screen_height-100, TextManager::LEFT, TextManager::LAYER_HUD);
							m_input_bar_back->set_image_width(m_input_bar->get_image_width() + 6);
							if (m_configuration->get_bool_value("text_background")) {
								m_input_bar_back->set_invisible(false);
							}
						}
						if (event.key.keysym.sym == m_key_bindings.open_console || event.key.keysym.sym == m_alt_key_bindings.open_console) {
							m_chat_log->set_visible(true);
						}
					} else if (event.key.keysym.sym == m_key_bindings.open_team_chat || event.key.keysym.sym == m_alt_key_bindings.open_team_chat) {
						SDL_EnableUNICODE(1);
						SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);
						m_text_manager->set_active_color(TEXT_COLOR);
						m_text_manager->set_active_font(m_font);
						m_input_text = "[TEAM]> ";
						if (m_input_bar == NULL) {
							m_input_bar = m_text_manager->place_string(m_input_text, 20, m_screen_height-100, TextManager::LEFT, TextManager::LAYER_HUD);
							m_input_bar_back->set_image_width(m_input_bar->get_image_width() + 6);
							if (m_configuration->get_bool_value("text_background")) {
								m_input_bar_back->set_invisible(false);
							}
						}
					} else if (event.key.keysym.sym == m_key_bindings.show_menu || event.key.keysym.sym == m_alt_key_bindings.show_menu) {
						if (!m_chat_log->is_invisible()) {
							m_chat_log->set_visible(false);
						} else if (m_game_state == SHOW_MENUS && !m_players.empty()) {
							m_game_state = GAME_IN_PROGRESS;
						} else {
							reset_options();
							m_game_state = SHOW_MENUS;
						}
					}
				}
				break;

			case SDL_KEYUP:
				break;
				
			case SDL_MOUSEMOTION:
				// Use: event.motion.xrel, event.motion.yrel (changes in position), event.motion.x, event.motion.y
				m_mouse_x = event.motion.x;
				m_mouse_y = event.motion.y;
				m_crosshairs->set_x(m_mouse_x);
				m_crosshairs->set_y(m_mouse_y);
				if (!m_server_browser->is_invisible()) {
					m_server_browser->scrollbar_motion_event(event.motion);
				}
				if (!m_overlay_scrollbar->is_invisible()) {
					m_overlay_scrollbar->mouse_motion_event(event.motion);
				}
				if (!m_chat_log->is_invisible()) {
					m_chat_log->scrollbar_motion_event(event.motion);
				}
				
				if (m_game_state == SHOW_MENUS) {
					m_main_menu.mouse_motion_event(event.motion);
					if (!m_network.is_connected() || !m_join_sent_time == 0) {
						m_item_resume->set_state(MenuItem::DISABLED);
						m_item_disconnect->set_state(MenuItem::DISABLED);
					}
				} else if (m_game_state == SHOW_OPTIONS_MENU) {
					map<string, Text*>::iterator it;
					for ( it=m_options_menu_items.begin() ; it != m_options_menu_items.end(); it++ ) {
						Text* thisitem = (*it).second;
						double x = thisitem->get_x();
						double y = thisitem->get_y();
						if (m_mouse_x >= x && m_mouse_x <= x + thisitem->get_image_width()
						    && m_mouse_y >= y && m_mouse_y <= y + thisitem->get_image_height()) {
							// We're hovering over this menu item.
							thisitem->set_color(BUTTON_HOVER_COLOR);
						} else {
							thisitem->set_color(TEXT_COLOR);
						}
					}
				}
				break;
				
			case SDL_MOUSEBUTTONDOWN:
				// Firing code, use event.button.button, event.button.x, event.button.y
				process_mouse_click(event);
				if (!m_server_browser->is_invisible()) {
					m_server_browser->scrollbar_button_event(event.button);
				}
				if (!m_overlay_scrollbar->is_invisible()) {
					m_overlay_scrollbar->mouse_button_event(event.button);
				}
				if (!m_chat_log->is_invisible()) {
					m_chat_log->scrollbar_button_event(event.button);
				}
				break;
				
			case SDL_MOUSEBUTTONUP:
				process_mouse_click(event);
				if (!m_server_browser->is_invisible()) {
					m_server_browser->scrollbar_button_event(event.button);
				}
				if (!m_overlay_scrollbar->is_invisible()) {
					m_overlay_scrollbar->mouse_button_event(event.button);
				}
				if (!m_chat_log->is_invisible()) {
					m_chat_log->scrollbar_button_event(event.button);
				}
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
/*
 * Set which keys are used for which functions.
 */
void GameController::initialize_key_bindings() {
	// -1 = unused
	m_key_bindings.quit = -1;
	m_key_bindings.jump = SDLK_SPACE;
	m_key_bindings.show_overlay = SDLK_TAB;
	m_key_bindings.show_menu = SDLK_ESCAPE;
	m_key_bindings.open_chat = SDLK_t;
	m_key_bindings.open_team_chat = SDLK_y;
	m_key_bindings.open_console = SDLK_BACKQUOTE;
	m_key_bindings.send_chat = SDLK_RETURN;
	m_key_bindings.weapon_1 = SDLK_1;
	m_key_bindings.weapon_2 = SDLK_2;
	m_key_bindings.weapon_3 = SDLK_3;

	m_alt_key_bindings.quit = -1;
	m_alt_key_bindings.jump = -1;
	m_alt_key_bindings.show_overlay = -1;
	m_alt_key_bindings.show_menu = -1;
	m_alt_key_bindings.open_chat = -1;
	m_alt_key_bindings.open_team_chat = -1;
	m_alt_key_bindings.open_console = -1;
	m_alt_key_bindings.send_chat = SDLK_KP_ENTER;
	m_alt_key_bindings.weapon_1 = -1;
	m_alt_key_bindings.weapon_2 = -1;
	m_alt_key_bindings.weapon_3 = -1;
}

/*
 * Used to process any keys that are to be held down rather than pressed once.
 */
void GameController::parse_key_input() {
	// For keys that can be held down:
   	m_keys = SDL_GetKeyState(NULL);
   	
   	if (m_game_state == GAME_IN_PROGRESS && m_input_bar == NULL) {
		if ((m_key_bindings.jump != -1 && m_keys[m_key_bindings.jump]) || (m_alt_key_bindings.jump != -1 && m_keys[m_alt_key_bindings.jump])) {
			attempt_jump();
		}
		if ((m_key_bindings.weapon_1 != -1 && m_keys[m_key_bindings.weapon_1]) || (m_alt_key_bindings.weapon_1 != -1 && m_keys[m_alt_key_bindings.weapon_1])) {
			change_weapon(1);
		}
		if ((m_key_bindings.weapon_2 != -2 && m_keys[m_key_bindings.weapon_2]) || (m_alt_key_bindings.weapon_2 != -2 && m_keys[m_alt_key_bindings.weapon_2])) {
			change_weapon(2);
		}
		if ((m_key_bindings.weapon_3 != -3 && m_keys[m_key_bindings.weapon_3]) || (m_alt_key_bindings.weapon_3 != -3 && m_keys[m_alt_key_bindings.weapon_3])) {
			change_weapon(3);
		}
	}
	
	if ((m_key_bindings.show_overlay != -1 && m_keys[m_key_bindings.show_overlay]) || (m_alt_key_bindings.show_overlay != -1 && m_keys[m_alt_key_bindings.show_overlay])) {
		if (m_game_state == GAME_IN_PROGRESS) {
			if (m_overlay_background->is_invisible() == true) {
				toggle_score_overlay(true);
			}
		} else if (m_game_state != GAME_OVER) {
			if (m_overlay_background->is_invisible() == false) {
				toggle_score_overlay(false);
			}
		}
	} else {
		if (m_game_state != GAME_OVER && m_overlay_background->is_invisible() == false) {
			toggle_score_overlay(false);
		}
	}
}

/*
 * Process a mouse click, depending on the game state.
 */
void GameController::process_mouse_click(SDL_Event event) {
	switch (m_game_state) {
	case SHOW_MENUS: {
		if (event.button.button != 1) {
			return;
		}
		MenuItem* item = m_main_menu.mouse_button_event(event.button);
		if (item && event.type == SDL_MOUSEBUTTONUP) {
			item->set_state(MenuItem::NORMAL);
			m_sound_controller->play_sound("click");
			if (item->get_value() == "quit") {
				m_quit_game = true;
			} else if (item->get_value() == "resume") {
				if (!m_players.empty()) {
					m_game_state = GAME_IN_PROGRESS;
				} else {
					display_message("Not connected to server.");
				}
			} else if (item->get_value() == "submenu:options") {
				m_game_state = SHOW_OPTIONS_MENU;
			} else if (item->get_value() == "disconnect") {
				if (!m_players.empty()) {
					disconnect();
				} else {
					display_message("Not connected to server.");
				}
			} else if (item->get_value() == "connect") {
				m_game_state = SHOW_SERVER_BROWSER;
			}
		}
	}
	break;
	case SHOW_OPTIONS_MENU: {
		if (event.button.button != 1 || event.type != SDL_MOUSEBUTTONUP) {
			return;
		}
		// Check each item in the options menu.
		map<string, Text*>::iterator it;
		for ( it=m_options_menu_items.begin() ; it != m_options_menu_items.end(); it++ ) {
			Graphic* thisitem = (*it).second;
			double x = thisitem->get_x();
			double y = thisitem->get_y();
			if (event.button.x >= x && event.button.x <= x + thisitem->get_image_width()
			    && event.button.y >= y && event.button.y <= y + thisitem->get_image_height()) {
				if ((*it).first == "Back") {
					reset_options();
					m_game_state = SHOW_MENUS;
				} else if ((*it).first == "Enter Name") {
					// Open the input bar and allow the name to be entered.
					// Should replace later, to use a separate text entry location.
					SDL_EnableUNICODE(1);
					m_text_manager->set_active_color(TEXT_COLOR);
					m_text_manager->set_active_font(m_font);
					m_input_text = "> /name ";
					m_text_manager->remove_string(m_input_bar);
					m_input_bar = m_text_manager->place_string(m_input_text, 20, m_screen_height-100, TextManager::LEFT, TextManager::LAYER_HUD);
					m_input_bar_back->set_image_width(m_input_bar->get_image_width() + 6);
					if (m_configuration->get_bool_value("text_background")) {
						m_input_bar_back->set_invisible(false);
					}
				} else if ((*it).first == "Toggle Sound") {
					string sound = "";
					if (m_sound_controller->is_sound_on()) {
						m_sound_controller->set_sound_on(false);
						sound = "Sound: Off";
					} else {
						m_sound_controller->set_sound_on(true);
						sound = "Sound: On";
					}
					m_text_manager->remove_string(m_options_menu_items["Toggle Sound"]);
					m_text_manager->set_active_font(m_menu_font);
					m_options_menu_items["Toggle Sound"] = m_text_manager->place_string(sound, 50, 240, TextManager::LEFT, TextManager::LAYER_HUD);
				} else if ((*it).first == "Resolution" || (*it).first == "CurrResolution") {
					m_resolution_selected++;
					if (m_resolution_selected >= m_num_resolutions) {
						m_resolution_selected = 0;
					}
					int width = m_resolutions[m_resolution_selected].first;
					int height = m_resolutions[m_resolution_selected].second;
					stringstream resolution;
					resolution << width << "x" << height;
					m_text_manager->remove_string(m_options_menu_items["CurrResolution"]);
					m_text_manager->set_active_font(m_menu_font);
					m_options_menu_items["CurrResolution"] = m_text_manager->place_string(resolution.str(), 410, 280, TextManager::LEFT, TextManager::LAYER_HUD);
				} else if ((*it).first == "Fullscreen") {
					string fullscreen = "";
					if (m_fullscreen) {
						m_fullscreen = false;
						fullscreen = "Fullscreen: No";
					} else {
						m_fullscreen = true;
						fullscreen = "Fullscreen: Yes";
					}
					m_text_manager->remove_string(m_options_menu_items["Fullscreen"]);
					m_text_manager->set_active_font(m_menu_font);
					m_options_menu_items["Fullscreen"] = m_text_manager->place_string(fullscreen, 50, 320, TextManager::LEFT, TextManager::LAYER_HUD);
				} else if ((*it).first == "Apply") {
					int width = m_resolutions[m_resolution_selected].first;
					int height = m_resolutions[m_resolution_selected].second;
					m_configuration->set_bool_value("sound", m_sound_controller->is_sound_on());
					
					if (width != m_configuration->get_int_value("screen_width") || 
							height != m_configuration->get_int_value("screen_height") ||
							m_fullscreen != m_configuration->get_bool_value("fullscreen")) {
						m_configuration->set_int_value("screen_width", width);
						m_configuration->set_int_value("screen_height", height);
						m_configuration->set_bool_value("fullscreen", m_fullscreen);
						m_restart = true;
						m_quit_game = true;
					} else {
						m_game_state = SHOW_MENUS;
					}
				}
				m_sound_controller->play_sound("click");
			}
		}
	}
	break;
	case SHOW_SERVER_BROWSER: {
		if (event.button.button != 1 || event.type != SDL_MOUSEBUTTONUP) {
			return;
		}
		m_sound_controller->play_sound("click");
		
		string button = m_server_browser->check_button_press(event.button.x, event.button.y);
		if (button == "Back") {
			// Back.
			m_game_state = SHOW_MENUS;
			m_server_browser->deselect();
		} else if (button == "Refresh") {
			// Refresh.
			m_server_browser->clear();
			m_server_browser->deselect();
			scan_all();
		} else if (button == "Connect") {
			// Connect.
			int selected_item = m_server_browser->get_selected_item();
			if (selected_item < 0 || selected_item > m_server_browser->get_count()) {
				return;
			}
			connect_to_server(selected_item);
			m_server_browser->set_visible(false);
			m_game_state = SHOW_MENUS;
		}
		
		if (m_server_browser->get_count() == 0) {
			return;
		}
		
		int selected_item = m_server_browser->check_item_select(event.button.x, event.button.y);
		
		if (selected_item < 0) {
			return;
		}		
		
		if (m_last_clicked > get_ticks() - DOUBLE_CLICK_TIME) {
			connect_to_server(selected_item);
			m_server_browser->set_visible(false);
			m_server_browser->deselect();
			m_game_state = SHOW_MENUS;
		}
	}
	break;
	case GAME_IN_PROGRESS: {
		if (event.type != SDL_MOUSEBUTTONDOWN) {
			return;
		} else if (!m_overlay_background->is_invisible()) {
			// Do nothing.
		} else if (!m_chat_log->is_invisible()) {
			// Do nothing.
		} else if (event.button.button == 1) {
			// Fire the gun if it's ready.
			if (m_last_fired != 0 && m_last_fired > get_ticks() - m_params.firing_delay) {
				return;
			}
			if (m_players.empty() || m_players[m_player_id].is_frozen()) {
				return;
			}
			double x_dist = (event.button.x + m_offset_x) - m_players[m_player_id].get_x();
			double y_dist = (event.button.y + m_offset_y) - m_players[m_player_id].get_y();
			double direction = atan2(y_dist, x_dist) * RADIANS_TO_DEGREES;
			// Cause recoil if the player is not hanging onto a wall.
			// XXX: This is bad test to see if a player's hanging onto a wall, since a player could be hovering in mid-space with no velocity.  In fact, that would be really bad, because then the player couldn't use recoil to get moving again.  We'll probably need to keep a flag in the Player class that specifies whether the player is hanging onto a wall or not. (-- andrew)
			if (m_players[m_player_id].get_x_vel() != 0 || m_players[m_player_id].get_y_vel() != 0) {
				m_players[m_player_id].set_velocity(m_players[m_player_id].get_x_vel() - m_params.firing_recoil * cos((direction) * DEGREES_TO_RADIANS), m_players[m_player_id].get_y_vel() - m_params.firing_recoil * sin((direction) * DEGREES_TO_RADIANS));
			}
			m_last_fired = get_ticks();
			fire_weapon(m_players[m_player_id].get_x(), m_players[m_player_id].get_y(), direction);
			m_sound_controller->play_sound("fire");
		}
	}
	}
	m_last_clicked = get_ticks();
}

/*
 * Reset the options menu, without applying the changes.
 */
void GameController::reset_options() {
	string fullscreen = "";
	string sound = "";
	m_text_manager->set_active_color(TEXT_COLOR);
	if (m_configuration->get_bool_value("fullscreen")) {
		m_fullscreen = true;
		fullscreen = "Fullscreen: Yes";
	} else {
		m_fullscreen = false;
		fullscreen = "Fullscreen: No";
	}
	if (m_configuration->get_bool_value("sound")) {
		m_sound_controller->set_sound_on(true);
		sound = "Sound: On";
	} else {
		m_sound_controller->set_sound_on(false);
		sound = "Sound: Off";
	}
	m_text_manager->remove_string(m_options_menu_items["Toggle Sound"]);
	m_text_manager->set_active_font(m_menu_font);
	m_options_menu_items["Toggle Sound"] = m_text_manager->place_string(sound, 50, 240, TextManager::LEFT, TextManager::LAYER_HUD);
	m_text_manager->remove_string(m_options_menu_items["Fullscreen"]);
	m_text_manager->set_active_font(m_menu_font);
	m_options_menu_items["Fullscreen"] = m_text_manager->place_string(fullscreen, 50, 320, TextManager::LEFT, TextManager::LAYER_HUD);
	m_text_manager->remove_string(m_options_menu_items["CurrResolution"]);
	stringstream resolution;
	for (size_t i = 0; i < m_num_resolutions; i++) {
		int width = m_resolutions[i].first;
		int height = m_resolutions[i].second;
		stringstream resolution;
		resolution << width << "x" << height;
		if (m_screen_width == width && m_screen_height == height) {
			m_resolution_selected = i;
			m_options_menu_items["CurrResolution"] = m_text_manager->place_string(resolution.str(), 410, 280, TextManager::LEFT, TextManager::LAYER_HUD);
		}
	}
}

/*
 * Do the movement of objects in a certain time scale.
 */
void GameController::move_objects(float timescale) {
	if (timescale > 1.0) {
		while (timescale > 1.0) {
			timescale -= 1.0;
			move_objects(1.0);
		}
	}
	if (m_players.empty()) {
		return;
	}

	GraphicalPlayer&	player(m_players[m_player_id]);
	
	Point	oldpos(player.get_x(), player.get_y());

	player.update_position(timescale);

	double	half_width = player.get_radius();
	double	half_height = player.get_radius();
	
	// Check if the player is hitting a map edge.
	if (player.get_x() - half_width < 0) {
		player.set_x(half_width);
		player.set_y(oldpos.y);
		if (player.is_frozen() && !player.is_invisible()) {
			player.bounce(0, 0.9);
		} else {
			player.stop();
			// Rotate to a good orientation:
			//rotate_towards_angle(0, ROTATION_ADJUST_SPEED);
		}
	} else if (player.get_x() + half_width > m_map_width) {
		player.set_x(m_map_width - half_width);
		player.set_y(oldpos.y);
		if (player.is_frozen() && !player.is_invisible()) {
			player.bounce(180, 0.9);
		} else {
			player.stop();
			// Rotate to a good orientation:
			//rotate_towards_angle(180, ROTATION_ADJUST_SPEED);
		}
	}
	
	if (player.get_y() - half_height < 0) {
		player.set_x(oldpos.x);
		player.set_y(half_height);
		if (player.is_frozen() && !player.is_invisible()) {
			player.bounce(90, 0.9);
		} else {
			player.stop();
			// Rotate to a good orientation:
			//rotate_towards_angle(90, ROTATION_ADJUST_SPEED);
		}
	} else if (player.get_y() + half_height > m_map_height) {
		player.set_x(oldpos.x);
		player.set_y(m_map_height - half_height);
		if (player.is_frozen() && !player.is_invisible()) {
			player.bounce(270, 0.9);
		} else {
			player.stop();
			// Rotate to a good orientation:
			//rotate_towards_angle(270, ROTATION_ADJUST_SPEED);
		}
	}

	Point	newpos(player.get_x(), player.get_y());
	double	radius = player.get_radius();
	Circle	player_circle(newpos, radius);
	Circle	old_player_circle(oldpos, radius);
	
	// Check each object for collisions with the player.
	const list<BaseMapObject*>& map_objects(m_map->get_objects());
	for (list<BaseMapObject*>::const_iterator it(map_objects.begin()); it != map_objects.end(); it++) {
		BaseMapObject*	thisobj = *it;

		if (!thisobj->is_intersectable()) {
			continue;
		}

		const Shape& shape(*thisobj->get_bounding_shape());
		double angle_of_incidence = 0;
		double newdist = -1;

		if (thisobj->is_interactive()) {
			newdist = shape.solid_intersects_circle(player_circle, &angle_of_incidence);
			angle_of_incidence = get_normalized_angle(angle_of_incidence + 180);
		} else if (thisobj->is_collidable()) {
			newdist = shape.boundary_intersects_circle(player_circle, &angle_of_incidence);
			angle_of_incidence = get_normalized_angle(angle_of_incidence + 180);
		}

		if (newdist != -1) {
			// We are intesecting with the object

			if (thisobj->is_collidable()) {
				double olddist = shape.boundary_intersects_circle(old_player_circle, NULL);
				if (newdist < olddist) {
					// We're moving closer to the object... COLLISION!
					thisobj->collide(*this, player, oldpos, angle_of_incidence);
				}
			}
			if (thisobj->is_interactive()) {
				thisobj->interact(*this, player);
			}

		} else if (thisobj->is_interactive() && thisobj->is_engaged()) {
			// We are no longer engaging the object, but we were previously
			thisobj->disengage(*this, player);
		}
	}
	
	// If the player rotation is presently being controlld by a transition, don't update it here
	if (!m_transition_manager.get_transition("player_rotation")) {
		player.update_rotation(timescale);
	}
	
	// Set the player position and radar position.
	m_radar->move_blip(m_player_id, player.get_x(), player.get_y());
	m_radar->recenter(player.get_x(), player.get_y());

	// Set name sprites visible/invisible. and move players.
	map<uint32_t, GraphicalPlayer>::iterator it;
	for ( it=m_players.begin() ; it != m_players.end(); it++ ) {
		GraphicalPlayer&	currplayer(it->second);
		if (currplayer.is_invisible()) {
			currplayer.get_name_sprite()->set_invisible(true);
		} else {
			currplayer.get_name_sprite()->set_invisible(false);
			m_text_manager->reposition_string(currplayer.get_name_sprite(), currplayer.get_x(), currplayer.get_y() - (currplayer.get_radius()+30), TextManager::CENTER);
		}
		
		if (currplayer.get_id() == m_player_id) {
			continue;
		}
		
		currplayer.update_position(timescale);
		m_radar->move_blip(currplayer.get_id(), currplayer.get_x(), currplayer.get_y());
	}
}

void GameController::rotate_towards_angle(double angle_of_incidence, uint64_t duration) {
	if (GraphicalPlayer* player = get_player_by_id(m_player_id)) {
		double	currangle = player->get_sprite()->get_rotation();
		double	adjusted_angle = get_normalized_angle(currangle + 90 - angle_of_incidence);
		double	newangle = currangle;
		if (adjusted_angle < 90) {
			newangle = currangle + (90 - adjusted_angle);
		} else if (adjusted_angle > 270) {
			newangle = currangle - (adjusted_angle - 270);
		}

		if (newangle != currangle && !m_transition_manager.get_transition("player_rotation")) {
			Transition* rotation_transition = new Transition(m_players[m_player_id].get_sprite(), &Graphic::set_rotation, new LinearCurve(currangle, newangle), get_ticks(), duration);
			rotation_transition->set_curve_ownership(true);
			m_transition_manager.add_transition(rotation_transition, "player_rotation", false, TransitionManager::DELETE);
		}
	}
}

/*
 * Try to jump off of an obstacle.
 */
void GameController::attempt_jump() {
	if (m_players.empty()) {
		return;
	}

	GraphicalPlayer& player = m_players[m_player_id];
	
	if (player.is_frozen() || player.is_invisible()) {
		return;
	}
	
	// The new velocities
	Vector	new_velocity(Vector::make_from_magnitude(6.0, get_crosshairs_angle()));
	double	new_rotation = (((double)rand() / ((double)(RAND_MAX)+1)) - 0.5) * RANDOM_ROTATION_SCALE;
	
	// Check if we're next to the side of the map.
	double half_width = player.get_radius();
	double half_height = player.get_radius();

	if (player.get_x() - half_width <= 5) {
		jump(new_velocity, new_rotation);
	} else if (player.get_x() + half_width >= m_map_width - 5) {
		jump(new_velocity, new_rotation);
	}
	
	if (player.get_y() - half_height <= 5) {
		jump(new_velocity, new_rotation);
	} else if (player.get_y() + half_height >= m_map_height - 5) {
		jump(new_velocity, new_rotation);
	}

	// Check if we're near any obstacles.
	Circle	player_circle(player.get_position(), player.get_radius()+5);
	const list<BaseMapObject*>& map_objects(m_map->get_objects());
	for (list<BaseMapObject*>::const_iterator it(map_objects.begin()); it != map_objects.end(); it++) {
		BaseMapObject*	map_obj = *it;

		if (!map_obj->is_jumpable() || !map_obj->is_intersectable()) {
			continue;
		}

		if (map_obj->get_bounding_shape()->boundary_intersects_circle(player_circle, NULL) != -1) {
			jump(new_velocity, new_rotation);
		}
	}
}


void GameController::jump(Vector new_velocity, double new_rotation) {
	GraphicalPlayer& player = m_players[m_player_id];

	//
	// Set the player in motion
	//
	player.set_velocity(new_velocity);
	player.set_rotational_vel(new_rotation);
	m_transition_manager.remove_transition(m_transition_manager.get_transition("player_rotation"));

	//
	// Find the nearest obstacle that we are jumping towards
	//
	Point	start_pos(player.get_position());
	Point	end_pos(start_pos + new_velocity.get_unit_vector() * (m_map_width + m_map_height));
	double	angle_of_incidence = 0;
	double	shortest_dist = numeric_limits<double>::infinity();

	const list<BaseMapObject*>& map_objects(m_map->get_objects());
	for (list<BaseMapObject*>::const_iterator it(map_objects.begin()); it != map_objects.end(); it++) {
		BaseMapObject*	map_obj = *it;

		if (!map_obj->is_jumpable() || !map_obj->is_intersectable()) {
			continue;
		}

		double		new_angle = 0;
		Point		new_point = map_obj->get_bounding_shape()->intersects_line(start_pos, end_pos, &new_angle);
		
		if (new_point.x == -1 && new_point.y == -1) {
			continue;
		}
		
		double		new_dist = Point::distance(start_pos, new_point);
		
		if (new_dist != -1 && new_dist < shortest_dist) {
			shortest_dist = new_dist;
			angle_of_incidence = new_angle;
		}
	}

	if (isinf(shortest_dist)) {
		// Try the map edge
		double	new_angle = 0;
		Point	new_point = m_map_polygon.intersects_line(start_pos, end_pos, &new_angle);
		
		if (new_point.x != -1 && new_point.y != -1) {
			shortest_dist = Point::distance(start_pos, new_point);
			angle_of_incidence = new_angle;
		}
	}

	if (finite(shortest_dist)) {
		//
		// Set our rotational velocity so that we will land on this obstacle in a good orientation (i.e. head not hitting the wall)
		//
		angle_of_incidence = get_normalized_angle(angle_of_incidence + 180);

		double	currangle = player.get_sprite()->get_rotation();
		double	adjusted_angle = get_normalized_angle(currangle + 90 - angle_of_incidence);
		double	newangle = currangle;
		if (adjusted_angle < 110) {
			newangle = currangle + (110 - adjusted_angle);
		} else if (adjusted_angle > 250) {
			newangle = currangle - (adjusted_angle - 250);
		}

		if (newangle != currangle) {
			double	time_till_hit = shortest_dist / new_velocity.get_magnitude();
			double	ideal_velocity = (newangle - currangle) / time_till_hit;

			if (ideal_velocity < -2.0)
				player.set_rotational_vel(-2.0);
			else if (ideal_velocity > 2.0)
				player.set_rotational_vel(2.0);
			else
				player.set_rotational_vel(ideal_velocity);
		}
	}
}

/*
 * Called when the current player fires his weapon.
 */
void GameController::fire_weapon(double start_x, double start_y, double direction) {
	if (m_players.empty()) {
		return;
	}
	
	const list<BaseMapObject*>& map_objects(m_map->get_objects());
	Point startpos = Point(start_x, start_y);
	
	double shortestdist = numeric_limits<double>::max();
	double end_x = -1;
	double end_y = -1;
	
	// Find the closest object that the shot will hit, if any.
	BaseMapObject*		hit_object = NULL;

	for (list<BaseMapObject*>::const_iterator it(map_objects.begin()); it != map_objects.end(); it++) {
		BaseMapObject*	thisobj = *it;

		if (!thisobj->is_shootable() || !thisobj->is_intersectable()) {
			continue;
		}

		const Shape& shape(*thisobj->get_bounding_shape());
		//double dist_to_obstacle = Point::distance(Point(start_x, start_y), Point(thisobj->get_sprite()->get_x() + thisobj->get_sprite()->get_image_width()/2, thisobj->get_sprite()->get_y() + thisobj->get_sprite()->get_image_height()/2)) + 100.0;
		double angle = 0;
		double dist_to_obstacle = m_map_width + m_map_height; // XXX: longer than it needs to be...
		Point endpos(start_x + dist_to_obstacle * cos(direction * DEGREES_TO_RADIANS), start_y + dist_to_obstacle * sin(direction * DEGREES_TO_RADIANS));
		
		Point newpoint = shape.intersects_line(startpos, endpos, NULL);
		
		if (newpoint.x == -1 && newpoint.y == -1) {
			continue;
		}
		
		double newdist = Point::distance(Point(start_x, start_y), newpoint);
		
		if (newdist != -1 && newdist < shortestdist) {
			shortestdist = newdist;
			end_x = newpoint.x;
			end_y = newpoint.y;
			hit_object = thisobj;
		}
	}
	
	// Now check if any players are closer.
	const GraphicalPlayer* hit_player = NULL;
	
	map<uint32_t, GraphicalPlayer>::iterator it;
	for ( it=m_players.begin() ; it != m_players.end(); it++ ) {
		const GraphicalPlayer& currplayer = (*it).second;
		if (currplayer.get_id() == m_player_id) {
			continue;
		}
		double playerdist = Point::distance(Point(start_x, start_y), Point(currplayer.get_x(), currplayer.get_y()));
		
		if (playerdist > shortestdist) {
			continue;
		}
		
		double this_end_x = start_x + playerdist * cos(direction * DEGREES_TO_RADIANS);
		double this_end_y = start_y + playerdist * sin(direction * DEGREES_TO_RADIANS);
		vector<double> closestpoint = closest_point_on_line(start_x, start_y, this_end_x, this_end_y, currplayer.get_x(), currplayer.get_y());
		
		if (closestpoint.size() == 0) {
			continue;
		}
		
		double dist = Point::distance(Point(currplayer.get_x(), currplayer.get_y()), Point(closestpoint.at(0), closestpoint.at(1)));
		
		// If the closest point was behind the beginning of the shot, it's not a hit.
		if (closestpoint.at(2) < 0) {
			continue;
		}
		
		// If the shot hit the player:
		if (dist < currplayer.get_radius()) {
			shortestdist = playerdist;
			end_x = closestpoint.at(0);
			end_y = closestpoint.at(1);
			hit_player = &currplayer;
			hit_object = NULL;
		}
	}
	
	// If we're still not hitting anything, find the nearest map edge where the shot will hit.
	if (end_x == -1 && end_y == -1) {
		double dist_to_obstacle = m_map_width + m_map_height;
		Point endpos = Point(start_x + dist_to_obstacle * cos(direction * DEGREES_TO_RADIANS), start_y + dist_to_obstacle * sin(direction * DEGREES_TO_RADIANS));
		Point newpoint = m_map_polygon.intersects_line(startpos, endpos, NULL);
	
		if (newpoint.x != -1 || newpoint.y != -1) {		
			double newdist = Point::distance(Point(start_x, start_y), newpoint);
	
			if (newdist != -1 && newdist < shortestdist) {
				shortestdist = newdist;
				end_x = newpoint.x;
				end_y = newpoint.y;
				hit_player = NULL;
				hit_object = NULL;
			}
		}
	}

	// If this shot hit a map object, tell the map object about it
	if (hit_object != NULL) {
		if (!hit_object->shot(*this, m_players[m_player_id], Point(end_x, end_y), direction)) {
			// Instead of absorbing the shot, the map object redirected it
			end_x = -1;
			end_y = -1;
		}
	}
	
	// Create the gun fired packet and send it, and display the shot hit point.
	PacketWriter gun_fired(GUN_FIRED_PACKET);
	gun_fired << m_player_id;
	gun_fired << start_x;
	gun_fired << start_y;
	gun_fired << direction;
	if (end_x != -1 || end_y != -1) {
		gun_fired << end_x;
		gun_fired << end_y;
		Graphic* this_shot = new Sprite(*m_shot);
		this_shot->set_x(end_x);
		this_shot->set_y(end_y);
		this_shot->set_scale_x(.1);
		this_shot->set_scale_y(.1);
		this_shot->set_invisible(false);
		pair<Graphic*, unsigned int> new_shot(this_shot, get_ticks() + SHOT_DISPLAY_TIME);
		m_shots.push_back(new_shot);
		m_window->register_graphic(this_shot);
	}
	
	// Switch to the gun with the muzzle flash.
	GraphicGroup* frontarm = (GraphicGroup*)m_players[m_player_id].get_sprite()->get_graphic("frontarm");
	frontarm->get_graphic("gun")->set_invisible(true);
	send_animation_packet("frontarm/gun", "invisible", true);
	frontarm->get_graphic("gun_fired")->set_invisible(false);
	send_animation_packet("frontarm/gun_fired", "invisible", false);
	
	m_network.send_packet(gun_fired);
	
	// If a player was hit, send a packet about it
	if (hit_player != NULL) {
		if (!hit_player->is_frozen()) {
			m_sound_controller->play_sound("hit");
		}
		send_player_shot(m_player_id, hit_player->get_id(), direction-180);
	}
}

/*
 * Set the player sprites visible or invisible.
 */
void GameController::set_players_visible(bool visible) {
	if (m_players.empty()) {
		return;
	}

	map<uint32_t, GraphicalPlayer>::iterator it;
	for ( it=m_players.begin() ; it != m_players.end(); it++ ) {
		const GraphicalPlayer& currplayer = (*it).second;
		if (currplayer.get_sprite() == NULL) {
			continue;
		}
		if (visible) {
			currplayer.get_sprite()->set_invisible(currplayer.is_invisible());
			currplayer.get_name_sprite()->set_invisible(currplayer.is_invisible());
			m_radar->set_blip_invisible(it->first,currplayer.is_invisible());
		} else {
			currplayer.get_sprite()->set_invisible(true);
			currplayer.get_name_sprite()->set_invisible(true);
			m_radar->set_blip_invisible(it->first,true);
		}
	}
}

/*
 * Show or hide the overlay.
 */
void GameController::toggle_score_overlay(bool visible) {
	update_individual_scores();
	m_overlay_background->set_invisible(!visible);
	m_overlay_scrollbar->set_invisible(!visible);
	m_overlay_scrollarea->set_invisible(!visible);
	map<string, Text*>::iterator it;
	for ( it=m_overlay_items.begin() ; it != m_overlay_items.end(); it++ ) {
		Graphic* thisitem = (*it).second;
		thisitem->set_invisible(!visible);
	}
}

/*
 * Show or hide the main menu
 */
void GameController::toggle_main_menu(bool visible) {
	m_main_menu.get_graphic_group()->set_invisible(!visible);
}

/*
 * Show or hide the options menu
 */
void GameController::toggle_options_menu(bool visible) {
	map<string, Text*>::iterator it;
	for (it = m_options_menu_items.begin(); it != m_options_menu_items.end(); ++it) {
		it->second->set_invisible(!visible);
	}
}

/*
 * Change team scores.
 */
void GameController::change_team_scores(int bluescore, int redscore) {
	m_text_manager->set_active_font(m_menu_font);
	m_text_manager->set_active_color(TEXT_COLOR);
	
	if (redscore != -1) {
		if (m_overlay_items.count("red score") != 0) {
			m_text_manager->remove_string(m_overlay_items["red score"]);
		}
		
		stringstream redscoreprinter;
		redscoreprinter << redscore;
	 	m_overlay_items["red score"] = m_text_manager->place_string(redscoreprinter.str(), m_overlay_items["red label"]->get_x() + m_overlay_items["red label"]->get_image_width() + 10, 115, TextManager::LEFT, TextManager::LAYER_HUD, TEXT_LAYER);
	}
	
	if (bluescore != -1) {
		if (m_overlay_items.count("blue score") != 0) {
			m_text_manager->remove_string(m_overlay_items["blue score"]);
		}
		
		stringstream bluescoreprinter;
		bluescoreprinter << bluescore;
		m_overlay_items["blue score"] = m_text_manager->place_string(bluescoreprinter.str(), m_overlay_items["blue label"]->get_x() + m_overlay_items["blue label"]->get_image_width() + 10, 115, TextManager::LEFT, TextManager::LAYER_HUD, TEXT_LAYER);
	}

	m_text_manager->set_active_font(m_font);
}

/*
 * Update individual scores.
 */
void GameController::update_individual_scores() {
	// Place all the players into one of two lists based on their team
	list<const GraphicalPlayer*>	blue_players;
	list<const GraphicalPlayer*>	red_players;

	if (m_players.empty()) {
		return;
	}

	for (map<uint32_t, GraphicalPlayer>::iterator it = m_players.begin(); it != m_players.end(); ++it) {
		if (it->second.get_sprite() != NULL && it->second.get_team() == 'A') {
			blue_players.push_back(&it->second);
		} else if (it->second.get_sprite() != NULL && it->second.get_team() == 'B') {
			red_players.push_back(&it->second);
		}
	}
	// Sort these lists by score
	blue_players.sort(Player::compare_by_score());
	red_players.sort(Player::compare_by_score());

	m_text_manager->set_active_font(m_medium_font);
	m_text_manager->set_active_color(TEXT_COLOR);
	
	int count = 0;
	
	for (list<const GraphicalPlayer*>::iterator it = blue_players.begin() ; it != blue_players.end(); ++it) {
		update_individual_score_line(count++, **it);
	}
	
	// Skip two lines between the two teams
	count += 2;
	
	for (list<const GraphicalPlayer*>::iterator it = red_players.begin() ; it != red_players.end(); ++it) {
		update_individual_score_line(count++, **it);
	}

	m_text_manager->set_active_font(m_font);
}

void GameController::update_individual_score_line(int count, const GraphicalPlayer& currplayer) {

	string playername = currplayer.get_name();
	stringstream idprinter;
	idprinter << currplayer.get_id();
	string playerid = idprinter.str();
	string playernameforscore = currplayer.get_name();
	string playerscore = playernameforscore.append("score");

	m_text_manager->set_active_color(currplayer.get_team() == 'A' ? BLUE_COLOR : RED_COLOR);
	m_text_manager->set_shadow_color(currplayer.get_team() == 'A' ? BLUE_SHADOW : RED_SHADOW);

	if (m_overlay_items.count(playerscore) != 0) {
		m_text_manager->remove_string(m_overlay_items[playerscore]);
	}
	if (m_overlay_items.count(playerid) != 0) {
		m_text_manager->remove_string(m_overlay_items[playerid]);
	}

	stringstream scoreprinter;
	scoreprinter << currplayer.get_score();
	m_overlay_items[playerid] = m_text_manager->place_string(playername, 10, count*25, TextManager::LEFT, TextManager::LAYER_HUD);
	m_overlay_items[playerscore] = m_text_manager->place_string(scoreprinter.str(), m_overlay_background->get_image_width()/2, count*25, TextManager::LEFT, TextManager::LAYER_HUD);

	m_overlay_items[playerid]->set_priority(TEXT_LAYER);
	m_window->unregister_hud_graphic(m_overlay_items[playerid]);
	m_overlay_scrollarea->get_group()->remove_graphic(playerid);
	m_overlay_scrollarea->get_group()->add_graphic(m_overlay_items[playerid], playerid);

	m_overlay_items[playerscore]->set_priority(TEXT_LAYER);
	m_window->unregister_hud_graphic(m_overlay_items[playerscore]);
	m_overlay_scrollarea->get_group()->remove_graphic(playerscore);
	m_overlay_scrollarea->get_group()->add_graphic(m_overlay_items[playerscore], playerscore);
	
	if (m_overlay_items[playerid]->get_y() + m_overlay_items[playerid]->get_image_height() + 2 > m_overlay_scrollarea->get_content_height()) {
		m_overlay_scrollarea->set_content_height(m_overlay_items[playerid]->get_y() + m_overlay_items[playerid]->get_image_height() + 2);
	}

	m_text_manager->set_shadow_color(TEXT_SHADOW);
}

void GameController::delete_individual_score(const GraphicalPlayer& currplayer) {
	stringstream idprinter;
	idprinter << currplayer.get_id();
	string playerid = idprinter.str();
	string playernameforscore = currplayer.get_name();
	string playerscore = playernameforscore.append("score");

	if (m_overlay_items.count(playerscore) != 0) {
		m_text_manager->remove_string(m_overlay_items[playerscore]);
	}
	if (m_overlay_items.count(playerid) != 0) {
		m_text_manager->remove_string(m_overlay_items[playerid]);
	}
	
	m_overlay_scrollarea->get_group()->remove_graphic(playerid);
	m_overlay_scrollarea->get_group()->remove_graphic(playerscore);
	m_overlay_items.erase(playerscore);
	m_overlay_items.erase(playerid);
}

/*
 * Send a player shot packet.
 */
void GameController::send_player_shot(uint32_t shooter_id, uint32_t hit_player_id, double angle) {
	PacketWriter player_shot(PLAYER_SHOT_PACKET);
	player_shot << shooter_id;
	player_shot << hit_player_id;
	player_shot << "default"; // TODO: send weapon name here
	player_shot << angle;
	
	m_network.send_packet(player_shot);
}

/*
 * Try to connect to a server.
 */
void GameController::connect_to_server(const IPAddress& server_address, char team) {
	if (!m_network.connect(server_address)) {
		ostringstream	errmsg;
		errmsg << "Error: Could not connect to server at " << server_address;
		display_message(errmsg.str());
		cerr << errmsg.str() << endl;
		return;
	}
	
	PacketWriter join_request(JOIN_PACKET);
	join_request << m_protocol_number;
	join_request << m_name;
	if (is_valid_team(team)) {
		join_request << team;
	}
	
	m_join_sent_time = get_ticks();
	
	m_network.send_packet(join_request);
}

/*
 * Try to connect to a server from the server list by number.
 */
void GameController::connect_to_server(int servernum) {
	disconnect();
	connect_to_server(m_server_browser->get_server_info(servernum));
}

/*
 * Send a disconnect packet.
 */
void GameController::disconnect() {
	m_item_resume->set_state(MenuItem::DISABLED);
	m_item_disconnect->set_state(MenuItem::DISABLED);
	if (!m_players.empty()) {
		PacketWriter leave_request(LEAVE_PACKET);
		leave_request << m_player_id;
		m_network.send_packet(leave_request);
		
		clear_players();
		m_game_state = SHOW_MENUS;
		m_player_id = 0;
		
		display_message("Disconnected.");
	}
	m_network.disconnect();
}

/*
 * When we receive a welcome packet.
 */
void GameController::welcome(PacketReader& reader) {
	int serverversion;
	int playerid;
	string playername;
	char team;
	
	reader >> serverversion >> playerid >> playername >> team;
	
	m_player_id = playerid;
	m_name = playername;

	cout << "Received welcome packet. Version: " << serverversion << ", Player ID: " << playerid << ", Name: " << playername << ", Team: " << team << endl;
	send_ack(reader);
	
	if (serverversion != m_protocol_number) {
		ostringstream serveraddress;
		serveraddress << "Error: Server has a different protocol. Server: ";
		serveraddress << serverversion;
		serveraddress << " You: ";
		serveraddress << m_protocol_number;
		display_message(serveraddress.str());
		
		disconnect();
	}
	
	ostringstream serveraddress;
	serveraddress << "Connected to server: ";
	serveraddress << format_ip_address(m_network.get_server_address(), true);
	display_message(serveraddress.str());
	
	m_join_sent_time = 0;
	
	m_item_resume->set_state(MenuItem::NORMAL);
	m_item_disconnect->set_state(MenuItem::NORMAL);
	
	clear_players();
	
	// Insert different name colors and sprites depending on team.
	if (team == 'A') {
		m_players.insert(pair<int, GraphicalPlayer>(m_player_id,GraphicalPlayer(m_name.c_str(), m_player_id, team, new GraphicGroup(blue_player), blue_sprite->get_width()/2, blue_sprite->get_height()/2)));
		m_text_manager->set_active_color(BLUE_COLOR);
		m_text_manager->set_shadow_color(BLUE_SHADOW);
		m_window->register_graphic(&blue_player);
	} else {
		m_players.insert(pair<int, GraphicalPlayer>(m_player_id,GraphicalPlayer(m_name.c_str(), m_player_id, team, new GraphicGroup(red_player), red_sprite->get_width()/2, red_sprite->get_height()/2)));
		m_text_manager->set_active_color(RED_COLOR);
		m_text_manager->set_shadow_color(RED_SHADOW);
		m_window->register_graphic(&red_player);
	}
	m_window->register_graphic(m_players[m_player_id].get_sprite());
	m_radar->add_blip(m_player_id, team, 0, 0);
	// Because our blip never gets "activated", and if the mode is aural, we won't get any alpha, so add some
	m_radar->set_blip_alpha(m_player_id, 1.0);
	
	m_players[m_player_id].set_radius(30);
	m_players[m_player_id].set_name_sprite(m_text_manager->place_string(m_players[m_player_id].get_name(), m_screen_width/2, (m_screen_height/2)-(m_players[m_player_id].get_radius()+30), TextManager::CENTER, TextManager::LAYER_MAIN));
	
	send_my_player_update();
	
	m_game_state = GAME_IN_PROGRESS;
	
	m_text_manager->set_shadow_color(TEXT_SHADOW);
}

/*
 * Add a player when we get an announce packet.
 */
void GameController::announce(PacketReader& reader) {
	unsigned int playerid;
	string playername;
	char team;
	
	if (m_players.empty()) {
		// WELCOME packet not received yet
		// do NOT send an ACK for this ANNOUNCE packet, so that the server will resend it, hopefully after the WELCOME has come in.
		return;
	}

	send_ack(reader);
	
	reader >> playerid >> playername >> team;
	
	string joinmsg = "";
	joinmsg.append(playername);
	joinmsg.append(" has joined the game!");
	if (team == 'A') {
		display_message(joinmsg, BLUE_COLOR, BLUE_SHADOW);
	} else {
		display_message(joinmsg, RED_COLOR, RED_SHADOW);
	}

	// Ignore announce packet for ourself, but still display join message (above)
	if (playerid == m_player_id) {
		return;
	}

	// Add a different sprite and name color depending on team.
	if (team == 'A') {
		m_players.insert(pair<int, GraphicalPlayer>(playerid,GraphicalPlayer((const char*)playername.c_str(), playerid, team, new GraphicGroup(blue_player))));
		m_text_manager->set_active_color(BLUE_COLOR);
		m_text_manager->set_shadow_color(BLUE_SHADOW);
	} else {
		m_players.insert(pair<int, GraphicalPlayer>(playerid,GraphicalPlayer((const char*)playername.c_str(), playerid, team, new GraphicGroup(red_player))));
		m_text_manager->set_active_color(RED_COLOR);
		m_text_manager->set_shadow_color(RED_SHADOW);
	}
	m_radar->add_blip(playerid,team,0,0);
	
	// Register the player sprite with the window
	m_window->register_graphic(m_players[playerid].get_sprite());
	m_players[playerid].set_name_sprite(m_text_manager->place_string(m_players[playerid].get_name(), m_players[playerid].get_x(), m_players[playerid].get_y()-(m_players[playerid].get_radius()+30), TextManager::CENTER, TextManager::LAYER_MAIN));
	m_players[playerid].set_radius(40);
	m_text_manager->set_shadow_color(TEXT_SHADOW);
}

/*
 * When we receive a player update.
 */
void GameController::player_update(PacketReader& reader) {
	if (m_players.empty()) {
		return;
	}

	uint32_t player_id;
	reader >> player_id;

	if (player_id == m_player_id) {
		// If the player update packet is for this player, send an ACK for it
		send_ack(reader);
	}

	GraphicalPlayer* currplayer = get_player_by_id(player_id);
	if (currplayer == NULL) {
		cerr << "Error: Received update packet for non-existent player " << player_id << endl;
		return;
	}

	currplayer->read_update_packet(reader);

	if (player_id == m_player_id) {
		update_health_bar();
	}

	// Update the radar and name sprite
	m_radar->move_blip(player_id, currplayer->get_x(), currplayer->get_y());
	m_radar->set_blip_invisible(player_id, currplayer->is_invisible());
	currplayer->get_name_sprite()->set_invisible(currplayer->is_invisible());
	
	// If invisible or frozen, set these things appropriately and show/hide the sprite.
	if (currplayer->is_invisible()) {
		currplayer->set_velocity(0, 0);
	} else {
		// Reposition the name sprite to reflect the player's new position
		m_text_manager->reposition_string(currplayer->get_name_sprite(), currplayer->get_x(), currplayer->get_y() - (currplayer->get_radius()+30), TextManager::CENTER);
	}
	
	if (m_radar->get_mode() == RADAR_ON) {
		m_radar->set_blip_alpha(player_id, currplayer->is_frozen() ? 0.5 : 1.0);
	}
}

/*
 * Send a player update packet.
 */
void GameController::send_my_player_update() {
	if (m_players.empty()) {
		return;
	}

	PacketWriter player_update(PLAYER_UPDATE_PACKET);
	m_players[m_player_id].write_update_packet(player_update);
	m_network.send_packet(player_update);
}

/*
 * Send a message packet.
 */
void GameController::send_message(string message) {
	if (m_players.empty()) {
		return;
	}
	strip_leading_trailing_spaces(message);
	if (message.empty()) {
		return;
	}

	if (message.find_first_of(':') != string::npos) {
		// Message to individual player
		string			recipient_name;
		string			message_part;
		StringTokenizer(message, ':', 2) >> recipient_name >> message_part;

		const GraphicalPlayer*	player = get_player_by_name(recipient_name.c_str());
		strip_leading_trailing_spaces(message_part);

		if (player && !message_part.empty()) {
			PacketWriter	message_writer(MESSAGE_PACKET);
			message_writer << m_player_id << player->get_id() << message_part;
			m_network.send_packet(message_writer);
			return;
		}
	}

	// Broadcast message to all players
	PacketWriter	message_writer(MESSAGE_PACKET);
	message_writer << m_player_id << "" << message;
	m_network.send_packet(message_writer);
}

/*
 * Send a team message packet.
 */
void GameController::send_team_message(string message) {
	if (m_players.empty()) {
		return;
	}
	strip_leading_trailing_spaces(message);
	if (message.empty()) {
		return;
	}

	PacketWriter	message_writer(MESSAGE_PACKET);
	message_writer << m_player_id << m_players[m_player_id].get_team() << message;
	m_network.send_packet(message_writer);
}
 

/*
 * Deal with receiving a leave packet.
 */
void GameController::leave(PacketReader& reader) {
	uint32_t	playerid;
	string		leave_message;
	reader >> playerid >> leave_message;
	
	if (GraphicalPlayer* player = get_player_by_id(playerid)) {
		 // If it's for ourselves, we were kicked. Quit with a message.
		if (playerid == m_player_id) {
			string leavemsg = "You were kicked!  Reason: ";
			leavemsg.append(leave_message);
			display_message(leavemsg);
			cout << "You were kicked!  Reason: " << leave_message << endl;
			disconnect();
			m_game_state = SHOW_MENUS;
			return;
		}
	
		string leavemsg = "";
		leavemsg.append(player->get_name());
		leavemsg.append(" has left the game.");
	
		// Display a message based on their team.
		if (player->get_team() == 'A') {
			display_message(leavemsg, BLUE_COLOR, BLUE_SHADOW);
		} else {
			display_message(leavemsg, RED_COLOR, RED_SHADOW);
		}
	
		m_text_manager->remove_string(player->get_name_sprite());
		m_window->unregister_graphic(player->get_sprite());
		m_radar->remove_blip(playerid);
		delete_individual_score(*player);
		delete player->get_sprite();
		m_players.erase(playerid);
	}
}

/*
 * Called when a gun fired packet is received.
 */
void GameController::gun_fired(PacketReader& reader) {
	unsigned int playerid;
	double start_x;
	double start_y;
	double rotation;
	double end_x;
	double end_y;
	reader >> playerid >> start_x >> start_y >> rotation >> end_x >> end_y;
	
	if (playerid == m_player_id) {
		return;
	}
	
	if (GraphicalPlayer* player = get_player_by_id(playerid)) {
		// Show a graphic for the shot.
		Graphic* this_shot = new Sprite(*m_shot);
		this_shot->set_x(end_x);
		this_shot->set_y(end_y);
		this_shot->set_invisible(false);
		this_shot->set_scale_x(.1);
		this_shot->set_scale_y(.1);
		pair<Graphic*, unsigned int> new_shot(this_shot, get_ticks() + SHOT_DISPLAY_TIME);
		m_shots.push_back(new_shot);
		m_window->register_graphic(this_shot);
	
		m_sound_controller->play_sound("fire");
		m_radar->activate_blip(playerid, get_ticks(), m_params.radar_blip_duration);
	}
}

/*
 * Called when a player shot packet is received.
 */
void GameController::player_shot(PacketReader& reader) {
	unsigned int shooter_id;
	unsigned int shot_id;
	unsigned long time_to_unfreeze;
	int new_health;
	double shot_angle;
	
	reader >> shooter_id >> shot_id >> time_to_unfreeze >> new_health >> shot_angle;
	
	GraphicalPlayer* shooter = get_player_by_id(shooter_id);
	GraphicalPlayer* shotplayer = get_player_by_id(shot_id);
	if (shooter == NULL || shotplayer == NULL) {
		return;
	}

	shotplayer->set_health(new_health);
	
	if (!shotplayer->is_frozen() && time_to_unfreeze != 0) {
		ostringstream message;
		
		if (shooter_id == m_player_id) {
			message << "You";
		} else {
			message << shooter->get_name();
		}
		
		if (shotplayer->get_team() == shooter->get_team()) { // TODO (when we finish game modes): only say "betrayed" if team-play is in effect.
			message << " betrayed ";
		} else {
			message << " shot ";
		}
		
		if (shot_id == m_player_id) {
			message << "you";
		} else {
			message << shotplayer->get_name();
		}
		
		message << ".";
		
		if (shooter->get_team() == 'A') {
			display_message(message.str(), BLUE_COLOR, BLUE_SHADOW);
		} else if (shooter->get_team() == 'B') {
			display_message(message.str(), RED_COLOR, RED_SHADOW);
		}
	}
	
	// If we were frozen, add to our velocity based on the shot, and freeze.
	if (shot_id == m_player_id) {
		if (!m_players[m_player_id].is_frozen() && time_to_unfreeze != 0) {
			m_frozen_status_rect->set_y(m_screen_height/2 + m_players[m_player_id].get_radius() + 15);
			m_frozen_status_rect_back->set_y(m_frozen_status_rect->get_y());
			m_frozen_status_text->set_y(m_frozen_status_rect->get_y());
			m_sound_controller->play_sound("freeze");
			m_players[m_player_id].set_is_frozen(true);
			if (m_radar->get_mode() == RADAR_ON) {
				m_radar->set_blip_alpha(m_player_id, 0.5);
			}
			m_time_to_unfreeze = get_ticks() + time_to_unfreeze;
			m_total_time_frozen = time_to_unfreeze;
		}
		update_health_bar();
		if (shot_angle != 0) {
			m_players[m_player_id].set_velocity(m_players[m_player_id].get_x_vel() - m_params.firing_recoil * cos((shot_angle) * DEGREES_TO_RADIANS), m_players[m_player_id].get_y_vel() - m_params.firing_recoil * sin((shot_angle) * DEGREES_TO_RADIANS));
		}
	}
}

/*
 * Called when a message is receieved.
 */
void GameController::message(PacketReader& reader) {
	uint32_t sender_id;
	string recipient;
	string message_text;
	
	reader >> sender_id >> recipient >> message_text;

	if (sender_id == 0) {
		// sender_id = 0 ==> From the server
		string message("[Server]: ");
		message.append(message_text);

		display_message(message);

	} else if (const GraphicalPlayer* sender = get_player_by_id(sender_id)) {
		string message(sender->get_name());
		message.append(": ");
		if (is_valid_team(recipient[0])) {
			// Team chat
			message.append("[TEAM]: ");
		}
		message.append(message_text);
		
		// Show the message in a color depending on the sender's team.
		if (sender->get_team() == 'A') {
			display_message(message, BLUE_COLOR, BLUE_SHADOW);
		} else {
			display_message(message, RED_COLOR, RED_SHADOW);
		}
	}
}

/*
 * Called when a gate update packet is received.
 */
void GameController::gate_update(PacketReader& reader) {
	uint32_t	acting_player_id; 	// Who triggered the gate change?
	char		team;			// Which team's gate is being updated
	double		progress;		// How much has the gate opened? 0 == fully closed .. 1 == fully open
	int		change_in_players;	// {-1,0,1} = the change in the # of players engaging the gate
	size_t		new_nbr_players;	// How many players are now engaging the gate

	reader >> acting_player_id >> team >> progress >> change_in_players >> new_nbr_players;
	
	GraphicalPlayer* myplayer = get_player_by_id(m_player_id);
	GraphicalPlayer* actingplayer = get_player_by_id(acting_player_id);
	if (myplayer == NULL) {
		return;
	}
	
	ostringstream message;
	
	if (actingplayer != NULL) {
		message << actingplayer->get_name();
		if (change_in_players == 1) {
			message << " engaged the ";
		} else if (change_in_players == -1) {
			message << " disengaged from the ";
		}
	}
	
	if (change_in_players != 0) {
		if (team == 'A') {
			message << "blue gate!";
			display_message(message.str(), RED_COLOR, RED_SHADOW);
		} else if (team == 'B') {
			message << "red gate!";
			display_message(message.str(), BLUE_COLOR, BLUE_SHADOW);
		}
	}
	
	if (change_in_players == 1 && new_nbr_players == 1) {
		// The gate just started opening.
		// Play a sound and set the warning visible.
		string soundname = "";
		if (team == myplayer->get_team()) {
			soundname = "gatelower";
		} else {
			soundname = "positivegatelower";
		}
		
		m_gate_lower_sounds[team - 'A'] = m_sound_controller->play_sound(soundname);
		if (team == m_players[m_player_id].get_team()) {
			m_gate_warning->set_invisible(false);
			m_gate_warning_time = get_ticks();
		}
	} else if (change_in_players == -1 && new_nbr_players == 0) {
		// The gate just started closing.
		// Hide the warning and stop the sounds.
	 	if (m_gate_lower_sounds[team - 'A'] != -1) {
	 		if (team == myplayer->get_team()) {
		 		m_gate_warning_time = 0;
		 		m_gate_warning->set_invisible(true);
		 	}
	 		m_sound_controller->halt_sound(m_gate_lower_sounds[team - 'A']);
	 	}
	}
	
	m_map->set_gate_progress(team, progress);
	
	double width = ((1-progress) * (GATE_STATUS_RECT_WIDTH-2)) + 2;
	if (team == 'A') {
		m_blue_gate_status_rect->set_image_width(width);
	} else if (team == 'B') {
		m_red_gate_status_rect->set_image_width(width);
	}
}

/*
 * When a game start packet is received.
 */
void GameController::game_start(PacketReader& reader) {
	if (m_players.empty()) {
		// WELCOME packet not received yet
		// do NOT send an ACK for this GAME_START packet, so that the server will resend it, hopefully after the WELCOME has come in.
		return;
	}

	/*
	 * Process the packet
	 */
	string 		map_name;
	int 		map_revision;
	bool		game_started;
	uint64_t	timeleft;
	reader >> map_name >> map_revision >> game_started >> timeleft;

	send_ack(reader);

	/*
	 * Tell the player what's going on
	 */
	ostringstream	message;
	if (game_started) {
		if (m_game_state == GAME_OVER) {
			m_game_state = GAME_IN_PROGRESS;
		}
		if (timeleft == numeric_limits<uint64_t>::max()) {
			message << "Game in progress.  You will spawn when the next game starts.";
		} else if (timeleft/1000 > 0) {
			message << "Game started! " << timeleft / 1000 << " seconds until spawn.";
		} else {
			message << "Game started!";
		}
		m_sound_controller->play_sound("begin");
			
		toggle_score_overlay(false);
	} else if (timeleft/1000 > 0) {
		message << "Game starts in " << timeleft/1000 << " seconds.";
	}
	
	if (!message.str().empty()) {
		display_message(message.str().c_str());
	}

	/*
	 * Load the map, if it has changed
	 */
	if (m_map->is_loaded(map_name.c_str(), map_revision)) {
		// Map already loaded - just reset it
		m_map->reset();
	} else {
		// This map/revision is not already loaded - let's try to load it
		if (!load_map(map_name.c_str(), map_revision)) {
			// Couldn't load - request it from the server
			request_map();
		}
	}
}

/*
 * Called when the game stop packet is received.
 */
void GameController::game_stop(PacketReader& reader) {
	char		winningteam;
	int 		teamascore;
	int		teambscore;
	
	reader >> winningteam >> teamascore >> teambscore;

	send_ack(reader);
	
	m_game_state = GAME_OVER;
	
	if (winningteam == '-') {
		display_message("DRAW");
	} else if (winningteam == m_players[m_player_id].get_team()) {
		display_message("VICTORY!");
		m_sound_controller->play_sound("victory");
	} else {
		display_message("DEFEAT!");
		m_sound_controller->play_sound("defeat");
	}
	
	change_team_scores(teamascore, teambscore);
	toggle_score_overlay(true);

	// Temporary score display
	ostringstream	score_msg;
	score_msg << "Blue: " << teamascore << " / Red: " << teambscore;
	display_message(score_msg.str().c_str());
	// End temporary score display

	// Reset the gates and set yourself invisible and frozen until respawn.
	m_map->reset_gates();
	m_blue_gate_status_rect->set_image_width(GATE_STATUS_RECT_WIDTH);
	m_red_gate_status_rect->set_image_width(GATE_STATUS_RECT_WIDTH);
	m_players[m_player_id].set_is_invisible(true);
	m_radar->set_blip_invisible(m_player_id,true);
	m_players[m_player_id].set_is_frozen(true);
	m_time_to_unfreeze = 0;
	m_total_time_frozen = 0;
	m_last_fired = 0;
}

/*
 * Called when a score update packet is received.
 */
void GameController::score_update(PacketReader& reader) {
	if (m_players.empty()) {
		// WELCOME packet not received yet
		// do NOT send an ACK for this SCORE_UPDATE packet, so that the server will resend it, hopefully after the WELCOME has come in.
		return;
	}

	std::string	subject;
	int		score;
	reader >> subject >> score;

	send_ack(reader);

	if (is_valid_team(subject[0])) {
		char	team = subject[0];
		if (team == 'A') {
			change_team_scores(score, -1);
		} else {
			change_team_scores(-1, score);
		}
		toggle_score_overlay(!m_overlay_background->is_invisible());
	} else if (GraphicalPlayer* player = get_player_by_id(atoi(subject.c_str()))) {
		player->set_score(score);
	}
}

/*
 * Called when an animation packet is received.
 */
void GameController::animation_packet(PacketReader& reader) {
	uint32_t	player_id;
	string		spritelist;
	string		field;
	int		value;
	
	reader >> player_id >> spritelist >> field >> value;
	
	StringTokenizer tokenizer(spritelist, '/');
	
	if (m_players.count(player_id) == 0) {
		return;
	}
	
	// Get the sprite to modify by going through the path separated by slashes.
	Graphic* the_sprite = NULL;
	while (tokenizer.has_more()) {
		string spritename = tokenizer.get_next();
	
		if (spritename == "all") {
			if (the_sprite == NULL) {
				the_sprite = m_players[player_id].get_sprite();
			}
		} else {
			if (the_sprite == NULL) {
				the_sprite = m_players[player_id].get_sprite()->get_graphic(spritename);
			} else {
				the_sprite = the_sprite->get_graphic(spritename);
			}
		}
	
		if (the_sprite == NULL) {
			return;
		}
	}
	
	if (field == "rotation") {
		the_sprite->set_rotation(value);
	} else if (field == "scale_x") {
		the_sprite->set_scale_x(value);
	} else if (field == "scale_y") {
		the_sprite->set_scale_y(value);
	} else if (field == "x") {
		the_sprite->set_x(value);
	} else if (field == "y") {
		the_sprite->set_y(value);
	} else if (field == "center_x") {
		the_sprite->set_center_x(value);
	} else if (field == "center_y") {
		the_sprite->set_center_y(value);
	} else if (field == "invisible") {
		the_sprite->set_invisible(value);
		m_players[player_id].get_sprite()->set_invisible(m_players[player_id].get_sprite()->is_invisible());
	}
}

/*
 * Called when a request denied packet is received.
 */
void GameController::request_denied(PacketReader& reader) {
	int		packet_type;
	string		reason;
	reader >> packet_type >> reason;

	if (packet_type == JOIN_PACKET) {
		string message = "Join denied! Reason: ";
		message.append(reason);
		display_message(message);
		cout << "Join denied!  Reason: " << reason << endl;
		disconnect();
		m_game_state = SHOW_MENUS;
	}
}

/*
 * Called when a player name change packet is received.
 */
void GameController::name_change(PacketReader& reader) {
	uint32_t	player_id;
	string		new_name;
	reader >> player_id >> new_name;

	if (GraphicalPlayer* player = get_player_by_id(player_id)) {
		ostringstream	msg;
		msg << player->get_name() << " is now known as " << new_name;

		delete_individual_score(m_players[player_id]);

		player->set_name(new_name.c_str());
		if (player_id == m_player_id) {
			m_configuration->set_string_value("name", new_name);
			m_name = new_name;
		}

		// Re-create the name sprite.
		if (player->get_team() == 'A') {
			display_message(msg.str().c_str(), BLUE_COLOR, BLUE_SHADOW);
			m_text_manager->set_active_color(BLUE_COLOR);
			m_text_manager->set_shadow_color(BLUE_SHADOW);
		} else {
			display_message(msg.str().c_str(), RED_COLOR, RED_SHADOW);
			m_text_manager->set_active_color(RED_COLOR);
			m_text_manager->set_shadow_color(RED_SHADOW);
		}
		m_text_manager->remove_string(player->get_name_sprite());
		player->set_name_sprite(m_text_manager->place_string(player->get_name(), player->get_x(), player->get_y()-(player->get_radius()+30), TextManager::CENTER, TextManager::LAYER_MAIN));
		m_text_manager->set_shadow_color(TEXT_SHADOW);
		update_individual_scores();
	}
}

/*
 * Called when a team change packet is received.
 */
void GameController::team_change(PacketReader& reader) {
	uint32_t	playerid;
	char		team;
	reader >> playerid >> team;

	if (GraphicalPlayer* player = get_player_by_id(playerid)) {
		delete_individual_score(m_players[playerid]);
	
		player->set_team(team);

		ostringstream	msg;
		msg << player->get_name() << " has switched teams";
		
		// Remove the name and sprite.
		m_text_manager->remove_string(m_players[playerid].get_name_sprite());
		m_window->unregister_graphic(m_players[playerid].get_sprite());
		m_radar->remove_blip(playerid);
		delete m_players[playerid].get_sprite();

		// Generate new graphics for it.
		if (team == 'A') {
			player->set_sprite(new GraphicGroup(blue_player));
			display_message(msg.str().c_str(), BLUE_COLOR, BLUE_SHADOW);
			m_text_manager->set_shadow_color(BLUE_SHADOW);
		} else {
			player->set_sprite(new GraphicGroup(red_player));
			display_message(msg.str().c_str(), RED_COLOR, RED_SHADOW);
			m_text_manager->set_shadow_color(RED_SHADOW);
		}
		m_radar->add_blip(playerid,team,0,0);
		
		m_window->register_graphic(m_players[playerid].get_sprite());
		m_players[playerid].set_name_sprite(m_text_manager->place_string(m_players[playerid].get_name(), m_players[playerid].get_x(), m_players[playerid].get_y()-(m_players[playerid].get_radius()+30), TextManager::CENTER, TextManager::LAYER_MAIN));
		update_individual_scores();
	}
}

/*
 * Send an animation packet.
 */
void GameController::send_animation_packet(string sprite, string field, int value) {
	PacketWriter animation_packet(PLAYER_ANIMATION_PACKET);
	animation_packet << m_player_id << sprite << field << value;
	
	m_network.send_packet(animation_packet);
}

/*
 * Send a gate hold packet.
 */
void GameController::send_gate_hold(bool holding) {
	PacketWriter gate_hold(GATE_UPDATE_PACKET);
	if (holding) {
		gate_hold << m_player_id << get_other_team(m_players[m_player_id].get_team()) << 1;
	} else {
		gate_hold << m_player_id << get_other_team(m_players[m_player_id].get_team()) << 0;
	}
	m_network.send_packet(gate_hold);
}

void GameController::set_gate_hold(bool holding_gate) {
	if (m_holding_gate != holding_gate) {
		m_holding_gate = holding_gate;
		send_gate_hold(m_holding_gate);
	}
	
}

/*
 * Send a name change packet.
 */
void GameController::send_name_change_packet(const char* new_name) {
	PacketWriter packet(NAME_CHANGE_PACKET);
	packet << m_player_id << new_name;
	m_network.send_packet(packet);
}

/*
 * Send a team change packet.
 */
void GameController::send_team_change_packet(char new_team) {
	PacketWriter packet(TEAM_CHANGE_PACKET);
	packet << m_player_id << new_team;
	m_network.send_packet(packet);
}

/*
 * Display a message on the screen.
 */
void GameController::display_message(string message, Color color, Color shadow) {
	m_text_manager->set_active_color(color);
	m_text_manager->set_shadow_color(shadow);
	m_text_manager->set_active_font(m_font);
	int y = 20 + (m_font->ascent() + m_font->descent() + 5) * m_messages.size();
	Text* message_sprite = m_text_manager->place_string(message, 20, y, TextManager::LEFT, TextManager::LAYER_HUD);
	if (!message_sprite) {
		return;
	}
	uint64_t currframe = get_ticks();
	Message new_message;
	new_message.message = message_sprite;
	new_message.transition = new Transition(new_message.message, &Graphic::set_y, new LinearCurve(y, y), currframe, 1);
	new_message.timeout = currframe + MESSAGE_DISPLAY_TIME;
	m_transition_manager.add_transition(new_message.transition, false, TransitionManager::KEEP);
	m_messages.push_back(new_message);
	m_chat_window_transition_y->change_curve(currframe, new LinearCurve(0, y + message_sprite->get_image_height() + 6 - m_chat_window_back->get_y()), 1);
	double max_w = m_chat_window_transition_x->get_curve()->get_end();
	if (max_w < message_sprite->get_image_width() + 6) {
		m_chat_window_transition_x->change_curve(currframe, new LinearCurve(0, message_sprite->get_image_width() + 6), 1);
	}
	m_chat_log->add_message(message, color, shadow);
	if (m_configuration->get_bool_value("text_background")) {
		m_chat_window_back->set_invisible(false);
	}
}

/*
 * Get a player by their ID.
 */
GraphicalPlayer* GameController::get_player_by_id(uint32_t player_id) {
	map<uint32_t, GraphicalPlayer>::iterator it(m_players.find(player_id));
	return it == m_players.end() ? NULL : &it->second;
}
const GraphicalPlayer* GameController::get_player_by_id(uint32_t player_id) const {
	map<uint32_t, GraphicalPlayer>::const_iterator it(m_players.find(player_id));
	return it == m_players.end() ? NULL : &it->second;
}

/*
 * Get a player by name.
 */
GraphicalPlayer* GameController::get_player_by_name(const char* name) {
	for (map<uint32_t, GraphicalPlayer>::iterator it(m_players.begin()); it != m_players.end(); ++it) {
		if (it->second.compare_name(name)) {
			return &it->second;
		}
	}
	return NULL;
}

/*
 * Send an ack packet.
 */
void	GameController::send_ack(const PacketReader& packet) {
	PacketWriter		ack_packet(ACK_PACKET);
	ack_packet << m_player_id << packet.packet_type() << packet.packet_id();
	m_network.send_packet(ack_packet);
}


void	GameController::server_info(const IPAddress& server_address, PacketReader& info_packet) {
	uint32_t	request_packet_id;
	uint64_t	scan_start_time;
	info_packet >> request_packet_id >> scan_start_time;

	if (request_packet_id != m_current_scan_id && request_packet_id != m_current_ping_id) {
		// From an old scan - ignore it
		return;
	}
	
	if (server_address == m_metaserver_address) {
		// A response from the meta server
		// Now send an info packet to the server specified in this packet, to measure ping time and get the most up-to-date information
		IPAddress	server_address;
		info_packet >> server_address;
		scan_server(server_address);
	} else {
		// A response from an actual server
		// Get the info on the server, and present it to the user
		int		server_protocol_version;
		string		current_map_name;
		int		team_count[2];
		int		max_players;
		uint64_t	uptime;
		string		server_name;
		string		server_location;
		info_packet >> server_protocol_version >> current_map_name >> team_count[0] >> team_count[1] >> max_players >> uptime >> server_name >> server_location;
		
		m_ping = get_ticks() - scan_start_time;
		if (request_packet_id == m_current_ping_id) {
			//cerr << "Ping: " << m_ping << " Framerate: " << m_framerate << endl;
			return;
		}
		
		//cerr << "Received INFO packet from " << format_ip_address(server_address, true) << ": Protocol=" << server_protocol_version << "; Map=" << current_map_name << "; Blue players=" << team_count[0] << "; Red players=" << team_count[1] << "; Ping time=" << get_ticks() - scan_start_time << "ms"  << "; Uptime=" << uptime << endl;
		
		if (server_protocol_version != m_protocol_number) {
			//cerr << "Server with different protocol found: " << format_ip_address(server_address, true) << ": Protocol=" << server_protocol_version << "; Map=" << current_map_name << "; Blue players=" << team_count[0] << "; Red players=" << team_count[1] << "; Ping time=" << get_ticks() - scan_start_time << "ms" << endl;
			return;
		}
		
		if (m_server_browser->is_invisible()) {
			return;
		}
		
		if (!m_server_browser->contains_ip(server_address)) {
			m_server_browser->add_entry(server_address, current_map_name, team_count, max_players, uptime, m_ping, server_name, server_location);
		}
	}
}


void	GameController::hole_punch_packet(const IPAddress& server_address, PacketReader& packet) {
	uint32_t	scan_id;
	packet >> scan_id;

	if (scan_id != m_current_scan_id || m_server_browser->contains_ip(server_address)) {
		return;
	}

	scan_server(server_address);
}


void	GameController::upgrade_available(const IPAddress& server_address, PacketReader& packet) {
	string		latest_version;
	packet >> latest_version;
	ostringstream message;
	message << "Version: " << latest_version;

	// TODO: rewrite
	/*if (m_main_menu_items.find("Newversion") != m_main_menu_items.end()) {
		m_text_manager->remove_string(m_main_menu_items["Newversion"]);
	}
	if (m_main_menu_items.find("Newversion2") != m_main_menu_items.end()) {
		m_text_manager->remove_string(m_main_menu_items["Newversion2"]);
	}
	m_text_manager->set_active_font(m_menu_font);
	m_main_menu_items["Newversion"] = m_text_manager->place_string("There is an upgrade available!", 440, 250, TextManager::LEFT, TextManager::LAYER_HUD);
	m_main_menu_items["Newversion2"] = m_text_manager->place_string(message.str(), 600, 300, TextManager::LEFT, TextManager::LAYER_HUD);*/
}

void	GameController::scan_all() {
	PacketWriter info_request_packet(INFO_PACKET);
	m_current_scan_id = info_request_packet.packet_id();
	info_request_packet << m_protocol_number << m_current_scan_id << get_ticks() << m_client_version;
	m_network.broadcast_packet(DEFAULT_PORTNO, info_request_packet);
	IPAddress localhostip;
	if (resolve_hostname(localhostip, "localhost", DEFAULT_PORTNO)) {
		m_network.send_packet_to(localhostip, info_request_packet);
	}
	m_network.send_packet_to(m_metaserver_address, info_request_packet);
}

void    GameController::check_for_upgrade() {
	PacketWriter packet(UPGRADE_AVAILABLE_PACKET);
	packet << m_client_version;
	m_network.send_packet_to(m_metaserver_address, packet);
}

void	GameController::scan_local_network() {
	PacketWriter info_request_packet(INFO_PACKET);
	m_current_scan_id = info_request_packet.packet_id();
	info_request_packet << m_protocol_number << m_current_scan_id << get_ticks();
	m_network.broadcast_packet(DEFAULT_PORTNO, info_request_packet);
}

void	GameController::contact_metaserver() {
	PacketWriter info_request_packet(INFO_PACKET);
	m_current_scan_id = info_request_packet.packet_id();
	info_request_packet << m_protocol_number << m_current_scan_id << get_ticks() << m_client_version;
	m_network.send_packet_to(m_metaserver_address, info_request_packet);
}

void	GameController::ping_server(const IPAddress& server_address) {
	PacketWriter info_request_packet(INFO_PACKET);
	m_current_ping_id = info_request_packet.packet_id();
	info_request_packet << m_protocol_number << m_current_ping_id << get_ticks();
	m_network.send_packet_to(server_address, info_request_packet);
}

void	GameController::scan_server(const IPAddress& server_address) {
	PacketWriter info_request_packet(INFO_PACKET);
	info_request_packet << m_protocol_number << m_current_scan_id << get_ticks();
	m_network.send_packet_to(server_address, info_request_packet);
}

void	GameController::set_player_name(string name) {
	m_name = name;
	m_configuration->set_string_value("name", name);
	if (m_network.is_connected()) {
		send_name_change_packet(m_name.c_str());
	}
}

void	GameController::clear_players() {
	if (!m_players.empty()) {
		map<uint32_t, GraphicalPlayer>::iterator it;
		for ( it=m_players.begin() ; it != m_players.end(); it++ ) {
			const GraphicalPlayer& currplayer = (*it).second;
			m_text_manager->remove_string(m_players[currplayer.get_id()].get_name_sprite());
			m_window->unregister_graphic(m_players[currplayer.get_id()].get_sprite());
			m_radar->remove_blip(currplayer.get_id());
			delete_individual_score(m_players[currplayer.get_id()]);
			delete m_players[currplayer.get_id()].get_sprite();
		}
	}
	m_players.clear();
}

void	GameController::sound_finished(int channel) {
	for (unsigned int i = 0; i < (sizeof(m_gate_lower_sounds)/sizeof(m_gate_lower_sounds[0])); i++) {
		if (m_gate_lower_sounds[i] == channel) {
			m_gate_lower_sounds[i] = -1;
		}
	}
}

bool	GameController::wants_restart() {
	return m_restart;
}

string	GameController::get_server_address() {
	if (m_network.is_connected() && m_join_sent_time == 0) {
		return format_ip_address(m_network.get_server_address(), true);
	} else {
		return "";
	}
}

string	GameController::format_time_from_millis(uint64_t milliseconds) {
	unsigned int uptimesecs = (milliseconds/1000);
	unsigned int uptimedays = uptimesecs/86400;
	uptimesecs -= uptimedays * 86400;
	unsigned int uptimehours = uptimesecs/3600;
	uptimesecs -= uptimehours * 3600;
	unsigned int uptimeminutes = uptimesecs/60;
	uptimesecs -= uptimeminutes * 60;
	ostringstream uptimestr;
	if (uptimedays != 0) {
		uptimestr << uptimedays << "d ";
	}
	uptimestr << uptimehours << ":";
	if (uptimeminutes < 10) {
		uptimestr << "0";
	}
	uptimestr << uptimeminutes << ":";
	if (uptimesecs < 10) {
		uptimestr << "0";
	}
	uptimestr << uptimesecs;
	return uptimestr.str();
}

void GameController::map_info_packet(PacketReader& reader) {
	if (m_map_receiver.get() && m_map_receiver->map_info(reader)) {
		init_map();
		if (m_map_receiver->is_done()) {
			m_map_receiver.reset();
		}
		send_ack(reader);
	}
}

void GameController::map_object_packet(PacketReader& reader) {
	if (m_map_receiver.get() && m_map_receiver->map_object(reader)) {
		if (m_map_receiver->is_done()) {
			m_map_receiver.reset();
		}
		send_ack(reader);
	}
}

void	GameController::request_map() {
	m_map->clear();
	PacketWriter		packet(MAP_INFO_PACKET);
	packet << m_player_id;
	m_network.send_packet(packet);
	m_map_receiver.reset(new MapReceiver(*m_map, packet.packet_id()));
}

bool	GameController::load_map(const char* map_name, int map_revision) {
	if (strpbrk(map_name, "/\\") != NULL) {
		return false;
	}

	string		map_filename(map_name);
	map_filename += ".map";

	if (!m_map->load_file(m_path_manager.data_path(map_filename.c_str(), "maps"))) {
		return false;
	}

	if (m_map->get_revision() != map_revision) {
		return false;
	}

	init_map();
	return true;
}

void	GameController::init_map() {
	m_map_width = m_map->get_width();
	m_map_height = m_map->get_height();
	m_map_polygon.make_rectangle(m_map_width, m_map_height);
}

void	GameController::display_legalese() {
	const char*	legalese[] = {
		"This is Leges Motus, a networked, 2D shooter set in zero gravity.",
		" ",
		"Copyright 2009 Andrew Ayer, Nathan Partlan, Jeffrey Pfau",
		" ",
		"Leges Motus is free and open source software.  You may redistribute it and/or",
		"modify it under the terms of version 2, or (at your option) version 3, of the",
		"GNU General Public License (GPL), as published by the Free Software Foundation.",
		" ",
		"Leges Motus is distributed in the hope that it will be useful, but WITHOUT ANY",
		"WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A",
		"PARTICULAR PURPOSE.  See the full text of the GNU General Public License for",
		"further detail.",
		" ",
		"For a full copy of the GNU General Public License, please see the COPYING file",
		"in the root of the source code tree.  You may also retrieve a copy from",
		"<http://www.gnu.org/licenses/gpl-2.0.txt>, or request a copy by writing to the",
		"Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA",
		"02111-1307  USA",
		" "
	};

	for (size_t i = 0; i < sizeof(legalese) / sizeof(legalese[0]); ++i) {
		display_message(legalese[i]);
	}
}

void GameController::game_param_packet(PacketReader& packet) {
	m_params.process_param_packet(packet);
	send_ack(packet);

	set_radar_mode(m_params.radar_mode);
	m_radar->set_scale(m_params.radar_scale);
}

void GameController::set_radar_mode(RadarMode mode) {
	if (mode == m_radar->get_mode()) {
		return;
	}

	m_radar->set_mode(mode);

	if (mode == RADAR_AURAL) {
		// Hide all the radar blips, because we're entering aural mode
		for (std::map<uint32_t, GraphicalPlayer>::const_iterator it(m_players.begin()); it != m_players.end(); ++it) {
			if (it->second.get_id() != m_player_id) {
				m_radar->set_blip_alpha(it->second.get_id(), 0.0);
			}
		}
	}
}

void GameController::change_weapon(int weaponnum) {
}

double	GameController::get_crosshairs_angle() const {
	if (m_players.empty()) {
		return 0;
	}

	Point	crosshairs_location(m_crosshairs->get_x() + m_offset_x, m_crosshairs->get_y() + m_offset_y);
	return Vector(crosshairs_location - get_player_by_id(m_player_id)->get_position()).get_angle();
}

