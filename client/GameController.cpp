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
#include "Weapon.hpp"
#include "GraphicMenuItem.hpp"
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
const uint64_t GameController::MUZZLE_FLASH_LENGTH = 80;
const int GameController::GATE_WARNING_FLASH_LENGTH = 3000;
const double GameController::RANDOM_ROTATION_SCALE = 1.0;
const Color GameController::BLUE_COLOR(0.6, 0.6, 1.0);
const Color GameController::RED_COLOR(1.0, 0.6, 0.6);
const Color GameController::BRIGHT_GREEN(0.0, 0.6, 0.0, 0.8);
const Color GameController::BRIGHT_ORANGE(1.0, 0.5, 0.0, 0.8);
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
const int GameController::ENERGY_BAR_WIDTH = 100;
const int GameController::COOLDOWN_BAR_WIDTH = 150;
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
	delete blue_back_arm;
	delete red_sprite;
	delete red_back_arm;
	delete m_crosshairs;

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
	delete m_energy_bar;
	delete m_energy_bar_back;
	
	delete m_input_bar_back;
	delete m_chat_window_back;
	
	delete m_weapon_selector;

	// TEMPORARY MAP CODE BY ANDREW
	delete m_map;

	clear_weapons();

	delete m_text_manager;
	m_sound_controller->destroy_instance();
	delete m_font;
	delete m_bold_font;
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

	m_last_damage_time = 0;
	m_last_recharge_time = 0;
	m_time_to_unfreeze = 0;
	m_total_time_frozen = 0;
	m_last_clicked = 0;
	m_muzzle_flash_start = 0;
	
	m_curr_weapon_image = NULL;
	m_weapon_selector = NULL;
	m_current_weapon = NULL;

	m_font = new Font(m_path_manager.data_path("JuraMedium.ttf", "fonts"), 14);
	m_bold_font = new Font(m_path_manager.data_path("JuraDemiBold.ttf", "fonts"), 14);
	m_bold_font->set_font_style(true, false);
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

	m_round_end_time = 0;

	// Initialize all of the components of the player sprites.
	blue_sprite = new Sprite(m_path_manager.data_path("blue_armless.png","sprites"));
	blue_back_arm = new Sprite(m_path_manager.data_path("blue_backarm.png","sprites"));
	blue_back_arm->set_center_x(27);
	blue_back_arm->set_center_y(29);
	blue_back_arm->set_x(-5);
	blue_back_arm->set_y(-20);
	blue_back_arm->set_priority(1);
	
	blue_player.add_graphic(blue_sprite, "torso");
	blue_player.add_graphic(blue_back_arm, "backarm");
	make_front_arm_graphic(blue_player, "blue_frontarm.png", NULL, NULL);
	blue_player.set_invisible(true);
	
	red_sprite = new Sprite(m_path_manager.data_path("red_armless.png","sprites"));
	red_back_arm = new Sprite(m_path_manager.data_path("red_backarm.png","sprites"));
	red_back_arm->set_center_x(27);
	red_back_arm->set_center_y(29);
	red_back_arm->set_x(-5);
	red_back_arm->set_y(-20);
	red_back_arm->set_priority(1);
	
	red_player.add_graphic(red_sprite, "torso");
	red_player.add_graphic(red_back_arm, "backarm");
	make_front_arm_graphic(red_player, "red_frontarm.png", NULL, NULL);
	red_player.set_invisible(true);
	
	m_crosshairs = new Sprite(m_path_manager.data_path("crosshairs.png", "sprites"));
	m_crosshairs->set_priority(-10);
	m_window->register_hud_graphic(m_crosshairs);
	
	m_shot = new Sprite(m_path_manager.data_path("shot.png", "sprites"));
	m_shot->set_invisible(true);
	
	m_logo = new Sprite(m_path_manager.data_path("legesmotuslogo.png", "sprites"));
	m_logo->set_x(m_screen_width/2);
	m_logo->set_y(100);
	m_logo->set_priority(0);
	m_window->register_hud_graphic(m_logo);
	
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
	m_version_nag1 = NULL;
	m_version_nag2 = NULL;
	
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
	ListMenuItem* current_lmi;
	m_options_menu.add_item(TextMenuItem::with_manager(m_text_manager, "Cancel", "cancel", 50, m_screen_height - 50));
	m_options_menu.add_item(TextMenuItem::with_manager(m_text_manager, "Enter Name", "name", 50, 200));
	current_lmi = new ListMenuItem("sound", TextMenuItem::with_manager(m_text_manager, "Toggle Sound:", "sound", 50, 240));
	current_lmi->add_option(TextMenuItem::with_manager(m_text_manager, "On", "on", 300, 240));
	current_lmi->add_option(TextMenuItem::with_manager(m_text_manager, "Off", "off", 300, 240));
	current_lmi->set_default_index(m_sound_controller->is_sound_on() ? 0 : 1);
	current_lmi->set_current_index(m_sound_controller->is_sound_on() ? 0 : 1);
	m_options_form.add_item("sound", current_lmi);
	m_options_menu.add_item(current_lmi);
	current_lmi = new ListMenuItem("fullscreen", TextMenuItem::with_manager(m_text_manager, "Fullscreen:", "fullscreen", 50, 320));
	current_lmi->add_option(TextMenuItem::with_manager(m_text_manager, "On", "on", 300, 320));
	current_lmi->add_option(TextMenuItem::with_manager(m_text_manager, "Off", "off", 300, 320));
	current_lmi->set_default_index(m_configuration->get_bool_value("fullscreen") ? 0 : 1);
	current_lmi->set_current_index(m_configuration->get_bool_value("fullscreen") ? 0 : 1);
	m_options_form.add_item("fullscreen", current_lmi);
	m_options_menu.add_item(current_lmi);
	m_options_menu.add_item(TextMenuItem::with_manager(m_text_manager, "Apply", "apply", m_screen_width - 200, m_screen_height - 50));
	current_lmi = new ListMenuItem("resolution", TextMenuItem::with_manager(m_text_manager, "Screen Resolution:", "resolution", 50, 280));
	m_options_form.add_item("resolution", current_lmi);
	m_options_menu.add_item(current_lmi);

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
		current_lmi->add_option(TextMenuItem::with_manager(m_text_manager, resolution.str(), resolution.str(), 400, 280));
		if (m_screen_width == width && m_screen_height == height) {
			current_lmi->set_current_index(i);
			current_lmi->set_default_index(i);
		}
	}
	m_window->register_hud_graphic(m_options_menu.get_graphic_group());
	toggle_options_menu(false);
	
	// Initialize the weapon selector menu.
	init_weapon_selector();
	
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
	m_overlay_scrollbar->set_length(m_overlay_background->get_row_height(2) - 20);
	m_overlay_scrollbar->set_x(m_overlay_background->get_x() + m_overlay_background->get_image_width()/2 - 20);
	m_overlay_scrollbar->set_y(m_overlay_background->get_y() + m_overlay_background->get_row_height(0) + m_overlay_background->get_row_height(1) + 5 + m_overlay_scrollbar->get_length()/2);
	m_overlay_scrollbar->set_section_color(ScrollBar::BUTTONS, Color(0.7,0.2,0.1));
	m_overlay_scrollbar->set_section_color(ScrollBar::TRACK, Color(0.2,0.1,0.1));
	m_overlay_scrollbar->set_section_color(ScrollBar::TRACKER, Color(0.2,0.2,0.4));
	m_overlay_scrollbar->set_scroll_speed(3);

	m_overlay_scrollarea = new ScrollArea(m_overlay_background->get_image_width(), m_overlay_background->get_row_height(2) - 30, m_overlay_background->get_image_width(), 10, NULL, m_overlay_scrollbar);
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

	// Initialize the energy bar.
	m_energy_text = NULL;
	m_energy_bar = new TableBackground(1, ENERGY_BAR_WIDTH);
	m_energy_bar->set_row_height(0, STATUS_BAR_HEIGHT);
	m_energy_bar->set_priority(-1);
	m_energy_bar->set_cell_color(0, BRIGHT_GREEN);
	m_energy_bar->set_x(ENERGY_BAR_WIDTH);
	m_energy_bar->set_y(m_screen_height - m_energy_bar->get_image_height() - 20);
	m_window->register_hud_graphic(m_energy_bar);
	m_energy_bar_back = new TableBackground(1, ENERGY_BAR_WIDTH);
	m_energy_bar_back->set_row_height(0, STATUS_BAR_HEIGHT);
	m_energy_bar_back->set_priority(0);
	m_energy_bar_back->set_cell_color(0, Color(0.1, 0.1, 0.1, 0.5));
	m_energy_bar_back->set_x(m_energy_bar->get_x());
	m_energy_bar_back->set_y(m_energy_bar->get_y());
	m_window->register_hud_graphic(m_energy_bar_back);
	
	update_energy_bar(0);
	
	// Initialize the cooldown bar.
	m_cooldown_bar = new TableBackground(1, COOLDOWN_BAR_WIDTH);
	m_cooldown_bar->set_row_height(0, STATUS_BAR_HEIGHT/2);
	m_cooldown_bar->set_priority(-1);
	m_cooldown_bar->set_cell_color(0, BRIGHT_ORANGE);
	m_cooldown_bar->set_x(m_energy_bar->get_x() + ENERGY_BAR_WIDTH/2 + m_cooldown_bar->get_image_width()/2 + 20);
	m_cooldown_bar->set_y(m_energy_bar->get_y() + m_energy_bar->get_image_height() - m_cooldown_bar->get_image_height());
	m_window->register_hud_graphic(m_cooldown_bar);
	m_cooldown_bar_back = new TableBackground(1, COOLDOWN_BAR_WIDTH);
	m_cooldown_bar_back->set_row_height(0, STATUS_BAR_HEIGHT/2);
	m_cooldown_bar_back->set_priority(0);
	m_cooldown_bar_back->set_cell_color(0, Color(0.1, 0.1, 0.1, 0.5));
	m_cooldown_bar_back->set_x(m_cooldown_bar->get_x());
	m_cooldown_bar_back->set_y(m_cooldown_bar->get_y());
	m_window->register_hud_graphic(m_cooldown_bar_back);
	
	update_cooldown_bar(0);

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
			if (m_players[m_player_id].is_dead()) {
				// If we were dead (i.e. at 0 energy), reset our energy
				// Otherwise, we continue with the energy we had before getting killed
				m_players[m_player_id].reset_energy();
				update_energy_bar();
				m_last_damage_time = 0;
			}
			m_players[m_player_id].set_is_frozen(false);
			m_last_recharge_time = 0;
			recreate_name(get_player_by_id(m_player_id));
			if (m_radar->get_mode() == RADAR_ON) {
				m_radar->set_blip_alpha(m_player_id, 1.0);
			}
			m_time_to_unfreeze = 0;
			m_total_time_frozen = 0;
		}

		if (!m_players.empty() && m_players[m_player_id].is_damaged() && !m_players[m_player_id].is_dead() && !m_players[m_player_id].is_frozen()) {
			// Figure out how much energy to recharge, based on the time elapsed since the last recharge
			uint64_t		now = get_ticks();

			if (m_params.recharge_continuously || !m_last_damage_time || now - m_last_damage_time >= m_params.recharge_delay) {
				uint64_t	time_elapsed = 0;
				if (!m_params.recharge_continuously && m_last_damage_time) {
					time_elapsed = (now - m_last_damage_time) - m_params.recharge_delay;
				} else if (m_last_recharge_time) {
					time_elapsed = now - m_last_recharge_time;
				}

				m_last_damage_time = 0;
				m_last_recharge_time = now - time_elapsed % m_params.recharge_rate;

				int	recharge = m_params.recharge_amount * (time_elapsed / m_params.recharge_rate);
				if (recharge) {
					m_players[m_player_id].change_energy(recharge);
					update_energy_bar();
				}
			}
		}
		
		// Update movement twice as often as graphics.
		unsigned long currframe = get_ticks();
		if ((currframe - lastmoveframe) >= (delay/2)) {
			move_objects((get_ticks() - lastmoveframe) / delay); // scale all position changes to keep game speed constant. 
			
			if (m_time_to_unfreeze != 0) {
				m_frozen_status_rect->set_image_width(((m_time_to_unfreeze - get_ticks())/(double)m_total_time_frozen) * FROZEN_STATUS_RECT_WIDTH);
			}
			
			if (m_round_end_time) {
				// If round will end in less than 2 minutes, display a message.
				if (get_ticks() >= m_round_end_time - 120000 && lastmoveframe < m_round_end_time - 120000) {
					display_message("Round will end in TWO minutes.", RED_COLOR, RED_SHADOW, true);
				}

				// If round will end in less than 30 seconds, display a message.
				if (get_ticks() >= m_round_end_time - 30000 && lastmoveframe < m_round_end_time - 30000) {
					display_message("Round will end in THIRTY seconds.", RED_COLOR, RED_SHADOW, true);
				}
				
				static const string numeral_to_english[10] = { "ONE", "TWO", "THREE", "FOUR", "FIVE", "SIX", "SEVEN", "EIGHT", "NINE", "TEN" };

				for (int a = 10000; a >= 1000; a-=1000) {
					// If round will end in less than <a> seconds, display a message.
					if (get_ticks() >= m_round_end_time - a && lastmoveframe < m_round_end_time - a) {
						display_message("Round will end in " + numeral_to_english[a/1000-1] + " seconds.", RED_COLOR, RED_SHADOW, true);
					}
				}
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
			
			// Update the cooldown bar.
			update_cooldown_bar();
			
			// Uncomment if framerate is needed.
			// the framerate:
			m_framerate = (1000/(currframe - startframe));
			m_transition_manager.update(currframe);
			
			if (!m_players.empty()) {
				//cerr << m_players[m_player_id].get_sprite()->get_rotation() << ", ";
				m_players[m_player_id].set_rotation_degrees(m_players[m_player_id].get_sprite()->get_rotation());
								
				// Change gun sprite if muzzle flash is done.
				Graphic* frontarm = m_players[m_player_id].get_sprite()->get_graphic("frontarm");
				if (m_muzzle_flash_start != 0 && get_ticks() - m_muzzle_flash_start >= MUZZLE_FLASH_LENGTH && frontarm->get_graphic("normal")->is_invisible()) {
					frontarm->get_graphic("normal")->set_invisible(false);
					send_animation_packet("frontarm/normal", "invisible", false);
					frontarm->get_graphic("firing")->set_invisible(true);
					send_animation_packet("frontarm/firing", "invisible", true);
					m_muzzle_flash_start = 0;
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
				m_weapon_selector->get_graphic_group()->set_invisible(true);
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
				m_weapon_selector->get_graphic_group()->set_invisible(true);
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
				m_weapon_selector->get_graphic_group()->set_invisible(true);
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

void GameController::init_weapon_selector() {
	// Initialize the weapon selector menu.
	if (m_weapon_selector != NULL) {
		m_window->unregister_hud_graphic(m_weapon_selector->get_graphic_group());
		delete m_weapon_selector;
	}
	
	m_weapon_selector_back = new RadialBackground(1);
	m_weapon_selector_back->set_border_color(Color(0.0,0.0,0.0,0.0));
	m_weapon_selector_back->set_inner_radius(80.0);
	m_weapon_selector_back->set_outer_radius(70.0);
	m_weapon_selector_back->set_border_radius(3.0);
	m_weapon_selector_back->set_border_angle(3);
	m_weapon_selector_back->set_x(m_screen_width/2);
	m_weapon_selector_back->set_y(m_screen_height/2);
	m_weapon_selector_back->set_rotation(0);
	
	m_weapon_selector = new RadialMenu(m_weapon_selector_back, Color(0.2,0.2,0.6,0.8), Color(0.1,0.1,0.3,1));

	GraphicMenuItem *cur_item;
	for (map<std::string, Weapon*>::iterator it(m_weapons.begin()); it != m_weapons.end(); ++it) {
		Graphic* this_graphic = m_graphics_cache.new_graphic<Sprite>(m_path_manager.data_path(it->second->hud_graphic(), "sprites"));
		cur_item = new GraphicMenuItem(this_graphic, it->first);
		cur_item->set_scale(0.75);
		m_weapon_selector->add_item(cur_item);
	}
	
	m_weapon_selector->get_graphic_group()->set_invisible(true);
	m_window->register_hud_graphic(m_weapon_selector->get_graphic_group());
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
		m_energy_bar->set_invisible(true);
		m_energy_bar_back->set_invisible(true);
		m_energy_text->set_invisible(true);
		m_cooldown_bar->set_invisible(true);
		m_cooldown_bar_back->set_invisible(true);
		if (m_curr_weapon_image != NULL) {
			m_curr_weapon_image->set_invisible(true);
		}
		return;
	}
	
	m_energy_bar->set_invisible(!visible);
	m_energy_bar_back->set_invisible(!visible);
	m_energy_text->set_invisible(!visible);
	
	m_cooldown_bar->set_invisible(!visible);
	m_cooldown_bar_back->set_invisible(!visible);
	
	if (m_curr_weapon_image != NULL) {
		// Uncomment the following to show the current weapon image only when done switching
		m_curr_weapon_image->set_invisible(!visible);
//		if (m_last_weapon_switch == 0 || (get_ticks() - m_last_weapon_switch) > m_params.weapon_switch_delay) {
//			m_curr_weapon_image->set_invisible(!visible);
//		} else {
//			m_curr_weapon_image->set_invisible(true);
//		}
	}
	
	if (player->is_frozen() && !player->is_invisible() && m_total_time_frozen > 100) {
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
 * Update the energy bar.
 */
void GameController::update_energy_bar(int new_energy) {
	if (GraphicalPlayer* player = get_player_by_id(m_player_id)) {
		if (new_energy != -1) {
			player->set_energy(new_energy);
		} else {
			new_energy = player->get_energy();
		}
	} else if (new_energy == -1) {
		new_energy = 0;
	}
	
	m_energy_bar->set_image_width((ENERGY_BAR_WIDTH-2) * ((double)new_energy/(GraphicalPlayer::MAX_ENERGY)) + 2);
	
	if (m_energy_text != NULL) {
		m_text_manager->remove_string(m_energy_text);
	}
	
	// Re-initialize the label.
	m_text_manager->set_active_color(1.0, 1.0, 1.0);
	m_text_manager->set_active_font(m_font);
	
	ostringstream energystring;
	energystring << "E: " << new_energy;
	m_energy_text = m_text_manager->place_string(energystring.str(), m_energy_bar->get_x(), m_energy_bar->get_y() + STATUS_BAR_HEIGHT/2, TextManager::CENTER, TextManager::LAYER_HUD, TEXT_LAYER);
	m_energy_text->set_y(m_energy_bar->get_y() + STATUS_BAR_HEIGHT/2 - m_energy_text->get_image_height()/2);
}

/*
 * Update the cooldown bar.
 */
void GameController::update_cooldown_bar(double new_cooldown) {
	if (m_current_weapon != NULL) {
		if (new_cooldown == -1) {
			new_cooldown = m_current_weapon->get_remaining_cooldown()/((double)m_current_weapon->get_total_cooldown());
			if ((get_ticks() - m_last_weapon_switch) < m_params.weapon_switch_delay) {
				if (m_last_weapon_switch != 0 && new_cooldown < (m_params.weapon_switch_delay - (get_ticks() - m_last_weapon_switch))/(double)m_params.weapon_switch_delay) {
					new_cooldown = (m_params.weapon_switch_delay - (get_ticks() - m_last_weapon_switch))/(double)m_params.weapon_switch_delay;
				}
			}
		}
	} else if (new_cooldown == -1) {
		new_cooldown = 0;
	}
	
	m_cooldown_bar->set_image_width((COOLDOWN_BAR_WIDTH-2) * (1.0-new_cooldown) + 2);
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
				if (!m_weapon_selector->get_graphic_group()->is_invisible()) {
					m_weapon_selector->mouse_motion_event(event.motion);
				}
				
				if (m_game_state == SHOW_MENUS) {
					m_main_menu.mouse_motion_event(event.motion);
					// TODO: Move this. Why is it HERE?
					if (!m_network.is_connected() || !m_join_sent_time == 0) {
						m_item_resume->set_state(MenuItem::DISABLED);
						m_item_disconnect->set_state(MenuItem::DISABLED);
					}
				} else if (m_game_state == SHOW_OPTIONS_MENU) {
					m_options_menu.mouse_motion_event(event.motion);
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

	// Check if left mouse button is held down, for weapon firing code.
	int mouse_x = 0;
	int mouse_y = 0;
	if (SDL_GetMouseState(&mouse_x, &mouse_y)&SDL_BUTTON(1)) {
		if (m_game_state == GAME_IN_PROGRESS) {
			if (m_players.empty() || m_players[m_player_id].is_frozen() || !m_current_weapon) {
				// Do nothing. We don't have a current player or weapon.
			} else if (!m_overlay_background->is_invisible()) {
				// Do nothing. The overlay is up.
			} else if (!m_chat_log->is_invisible()) {
				// Do nothing. The chat log is up.
			} else if (m_current_weapon->is_continuous()) {
				// Fire the gun if it's ready.
				double x_dist = (mouse_x + m_offset_x) - m_players[m_player_id].get_x();
				double y_dist = (mouse_y + m_offset_y) - m_players[m_player_id].get_y();
				double direction = atan2(y_dist, x_dist);
				if (m_last_weapon_switch == 0 || (get_ticks() - m_last_weapon_switch) > m_params.weapon_switch_delay) {
					m_current_weapon->fire(m_players[m_player_id], *this, m_players[m_player_id].get_position(), direction);
				}
			}
		}
	}
	
	// Check if the right mouse button is held down, for weapon switching.
	if (SDL_GetMouseState(&mouse_x, &mouse_y)&SDL_BUTTON(3)) {
		if (GraphicalPlayer* player = get_player_by_id(m_player_id)) {
			if (m_weapons.size() > 0 && m_game_state == GAME_IN_PROGRESS && !player->is_invisible() && !player->is_frozen() && !player->is_dead()) {
				m_weapon_selector->get_graphic_group()->set_invisible(false);
			}
		}
	} else {
		if (m_game_state == GAME_IN_PROGRESS) {
			if (!m_weapon_selector->get_graphic_group()->is_invisible()) {
				MenuItem* over = m_weapon_selector->item_at_position(mouse_x, mouse_y);
				if (over != NULL) {
					change_weapon(over->get_name().c_str());
				}
			}
			m_weapon_selector->get_graphic_group()->set_invisible(true);
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
	m_key_bindings.weapon_4 = SDLK_4;
	m_key_bindings.weapon_5 = SDLK_5;
	m_key_bindings.weapon_6 = SDLK_6;
	m_key_bindings.weapon_7 = SDLK_7;
	m_key_bindings.weapon_8 = SDLK_8;

	m_alt_key_bindings.quit = -1;
	m_alt_key_bindings.jump = -1;
	m_alt_key_bindings.show_overlay = -1;
	m_alt_key_bindings.show_menu = -1;
	m_alt_key_bindings.open_chat = -1;
	m_alt_key_bindings.open_team_chat = -1;
	m_alt_key_bindings.open_console = -1;
	m_alt_key_bindings.send_chat = SDLK_KP_ENTER;
	m_alt_key_bindings.weapon_1 = SDLK_KP1;
	m_alt_key_bindings.weapon_2 = SDLK_KP2;
	m_alt_key_bindings.weapon_3 = SDLK_KP3;
	m_alt_key_bindings.weapon_4 = SDLK_KP4;
	m_alt_key_bindings.weapon_5 = SDLK_KP5;
	m_alt_key_bindings.weapon_6 = SDLK_KP6;
	m_alt_key_bindings.weapon_7 = SDLK_KP7;
	m_alt_key_bindings.weapon_8 = SDLK_KP8;
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
			change_weapon(0U);
		}
		if ((m_key_bindings.weapon_2 != -1 && m_keys[m_key_bindings.weapon_2]) || (m_alt_key_bindings.weapon_2 != -1 && m_keys[m_alt_key_bindings.weapon_2])) {
			change_weapon(1U);
		}
		if ((m_key_bindings.weapon_3 != -1 && m_keys[m_key_bindings.weapon_3]) || (m_alt_key_bindings.weapon_3 != -1 && m_keys[m_alt_key_bindings.weapon_3])) {
			change_weapon(2U);
		}
		if ((m_key_bindings.weapon_4 != -1 && m_keys[m_key_bindings.weapon_4]) || (m_alt_key_bindings.weapon_4 != -1 && m_keys[m_alt_key_bindings.weapon_4])) {
			change_weapon(3U);
		}
		if ((m_key_bindings.weapon_5 != -1 && m_keys[m_key_bindings.weapon_5]) || (m_alt_key_bindings.weapon_5 != -1 && m_keys[m_alt_key_bindings.weapon_5])) {
			change_weapon(4U);
		}
		if ((m_key_bindings.weapon_6 != -1 && m_keys[m_key_bindings.weapon_6]) || (m_alt_key_bindings.weapon_6 != -1 && m_keys[m_alt_key_bindings.weapon_6])) {
			change_weapon(5U);
		}
		if ((m_key_bindings.weapon_7 != -1 && m_keys[m_key_bindings.weapon_7]) || (m_alt_key_bindings.weapon_7 != -1 && m_keys[m_alt_key_bindings.weapon_7])) {
			change_weapon(6U);
		}
		if ((m_key_bindings.weapon_8 != -1 && m_keys[m_key_bindings.weapon_8]) || (m_alt_key_bindings.weapon_8 != -1 && m_keys[m_alt_key_bindings.weapon_8])) {
			change_weapon(7U);
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
				if (item->get_name() == "quit") {
					m_quit_game = true;
				} else if (item->get_name() == "resume") {
					if (!m_players.empty()) {
						m_game_state = GAME_IN_PROGRESS;
					} else {
						display_message("Not connected to server.");
					}
				} else if (item->get_name() == "submenu:options") {
					m_game_state = SHOW_OPTIONS_MENU;
				} else if (item->get_name() == "disconnect") {
					if (!m_players.empty()) {
						disconnect();
					} else {
						display_message("Not connected to server.");
					}
				} else if (item->get_name() == "connect") {
					m_game_state = SHOW_SERVER_BROWSER;
				}
			}
		}
		break;
		case SHOW_OPTIONS_MENU: {
			if (event.button.button != 1) {
				return;
			}
			MenuItem* item = m_options_menu.mouse_button_event(event.button);
			if (item && event.type == SDL_MOUSEBUTTONUP) {
				m_sound_controller->play_sound("click");
				if(item->get_name() == "cancel") {
					reset_options();
					m_game_state = SHOW_MENUS;
				} else if(item->get_name() == "name") {
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
				} else if(item->get_name() == "apply") {
					string resolution = m_options_form.get_item("resolution")->get_value();
					size_t xpos = resolution.find('x');
					int width;
					int height;
					istringstream wstring(resolution.substr(0, xpos));
					istringstream hstring(resolution.substr(xpos + 1));
					wstring >> width;
					hstring >> height;
					m_sound_controller->set_sound_on(m_options_form.get_item("sound")->get_value() == "on");
					bool fullscreen = m_options_form.get_item("fullscreen")->get_value() == "on";
					if (width != m_configuration->get_int_value("screen_width") || 
							height != m_configuration->get_int_value("screen_height") ||
							fullscreen != m_configuration->get_bool_value("fullscreen")) {
						m_configuration->set_int_value("screen_width", width);
						m_configuration->set_int_value("screen_height", height);
						m_configuration->set_bool_value("fullscreen", fullscreen);
						m_restart = true;
						m_quit_game = true;
					} else {
						m_game_state = SHOW_MENUS;
					}
				}
			}
		}
		break;
		case SHOW_SERVER_BROWSER: {
			if (event.button.button != 1 || event.type != SDL_MOUSEBUTTONUP) {
				return;
			}
		
			string button = m_server_browser->check_button_press(event.button.x, event.button.y);
		
			if (button != "") {
				m_sound_controller->play_sound("click");
			}
		
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
		
			m_sound_controller->play_sound("click");
		
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
				if (m_players.empty() || m_players[m_player_id].is_frozen() || !m_current_weapon)
					return;

				double x_dist = (event.button.x + m_offset_x) - m_players[m_player_id].get_x();
				double y_dist = (event.button.y + m_offset_y) - m_players[m_player_id].get_y();
				double direction = atan2(y_dist, x_dist);
				if (m_last_weapon_switch == 0 || (get_ticks() - m_last_weapon_switch) > m_params.weapon_switch_delay) {
					m_current_weapon->fire(m_players[m_player_id], *this, m_players[m_player_id].get_position(), direction);
				}
			}
		}
	}
	m_last_clicked = get_ticks();
}

/*
 * Reset the options menu, without applying the changes.
 */
void GameController::reset_options() {
	m_options_form.reset();
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
			player.set_is_grabbing_obstacle(true);
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
			player.set_is_grabbing_obstacle(true);
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
			player.set_is_grabbing_obstacle(true);
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
			player.set_is_grabbing_obstacle(true);
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
			// We are intersecting with the object

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
	
	//
	// Make sure the player is able to jump right now
	//
	if (player.is_frozen() || player.is_invisible() || !player.is_grabbing_obstacle()) {
		return;
	}

	double	jump_angle = get_crosshairs_angle();
	
	//
	// Calculate the new velocities
	//
	Vector	new_velocity(Vector::make_from_magnitude(m_params.jump_velocity, jump_angle));
	double	new_rotation = (((double)rand() / ((double)(RAND_MAX)+1)) - 0.5) * RANDOM_ROTATION_SCALE;

	//
	// Find the nearest obstacle that we are jumping towards
	//
	Circle	player_circle(player.get_position(), player.get_radius()+5);
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

		// See if we're colliding with this obstacle right now
		double		colliding_angle_of_incidence;
		if (map_obj->get_bounding_shape()->boundary_intersects_circle(player_circle, &colliding_angle_of_incidence) != -1) {
			double	angle_difference = get_normalized_angle(colliding_angle_of_incidence - to_degrees(jump_angle));
			if (angle_difference < 90 || angle_difference > 270) {
				// We are jumping RIGHT INTO the obstacle that we're already colliding with
				// Abort the jump, since it can take us nowhere
				// (If the jump weren't aborted, it would cause problems with active map obstacles)
				return;
			}
		}

		// See if our trajectory collides with this obstacle
		double		new_angle = 0;
		Point		new_point = map_obj->get_bounding_shape()->intersects_line(start_pos, end_pos, &new_angle);
		
		if (new_point.x == -1 && new_point.y == -1) {
			// Not in the trajectory - continue to next obstacle...
			continue;
		}
		
		// How far away is the obstacle from us?
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

	//
	// Set the player in motion
	//
	player.set_velocity(new_velocity);
	player.set_rotational_vel(new_rotation);
	m_transition_manager.remove_transition(m_transition_manager.get_transition("player_rotation"));

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

void GameController::show_muzzle_flash() {
	// Switch to the gun with the muzzle flash.
	Graphic*	frontarm = m_players[m_player_id].get_sprite()->get_graphic("frontarm");
	frontarm->get_graphic("firing")->set_invisible(false);
	send_animation_packet("frontarm/firing", "invisible", false);
	frontarm->get_graphic("normal")->set_invisible(true);
	send_animation_packet("frontarm/normal", "invisible", true);
	m_muzzle_flash_start = get_ticks();
}

void GameController::show_bullet_impact(Point position) {
	Graphic* this_shot = new Sprite(*m_shot);
	this_shot->set_x(position.x);
	this_shot->set_y(position.y);
	this_shot->set_scale_x(.1);
	this_shot->set_scale_y(.1);
	this_shot->set_invisible(false);
	pair<Graphic*, unsigned int> new_shot(this_shot, get_ticks() + SHOT_DISPLAY_TIME);
	m_shots.push_back(new_shot);
	m_window->register_graphic(this_shot);
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
	m_options_menu.get_graphic_group()->set_invisible(!visible);
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

	update_energy_bar(0);
	
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

	bool wasfrozen = currplayer->is_frozen();

	currplayer->read_update_packet(reader);

	if (wasfrozen != currplayer->is_frozen()) {
		recreate_name(currplayer);
	}

	if (player_id == m_player_id) {
		update_energy_bar();
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
void GameController::weapon_discharged(PacketReader& reader) {
	uint32_t	player_id;
	string		weapon_name;

	reader >> player_id >> weapon_name;

	if (player_id == m_player_id) {
		// Ignore discharge packets from ourself
		return;
	}

	Weapon*		weapon = get_weapon(weapon_name);
	Player*		player = get_player_by_id(player_id);

	if (!weapon || !player) {
		return;
	}

	weapon->discharged(*player, *this, reader);
}

/*
 * Called when a player shot packet is received.
 */
void GameController::player_hit(PacketReader& reader) {
	uint32_t	shooter_id;
	string		weapon_name;
	uint32_t	shot_player_id;
	bool		has_effect;

	reader >> shooter_id >> weapon_name >> shot_player_id >> has_effect;

	GraphicalPlayer*	shooter = get_player_by_id(shooter_id);
	GraphicalPlayer*	shot_player = get_player_by_id(shot_player_id);

	if (!shooter || !shot_player) {
		return;
	}

	if (shot_player_id == m_player_id) {
		if (Weapon* weapon = get_weapon(weapon_name)) {
			weapon->hit(*shot_player, *shooter, has_effect, *this, reader);
		}
	}
}

void GameController::player_died(PacketReader& reader) {
	uint32_t	dead_player_id;
	uint32_t	killer_id;
	uint64_t	time_to_unfreeze;

	reader >> dead_player_id >> killer_id >> time_to_unfreeze;
	
	GraphicalPlayer* dead_player = get_player_by_id(dead_player_id);
	GraphicalPlayer* killer = killer_id ? get_player_by_id(killer_id) : NULL;

	if (!dead_player) {
		return;
	}

	if (!dead_player->is_frozen() && time_to_unfreeze != 0) {
		ostringstream message;
		bool bold = false;
		if (killer_id == m_player_id) {
			message << "You";
			bold = true;
		} else if (killer) {
			message << killer->get_name();
		} else {
			message << "A map hazard";
		}
		
		if (killer && killer->get_team() == dead_player->get_team()) { // TODO (when we finish game modes): only say "betrayed" if team-play is in effect.
			message << " betrayed ";
		} else {
			message << " killed ";
		}
		
		if (dead_player_id == m_player_id) {
			message << "you";
			bold = true;
		} else {
			message << dead_player->get_name();
		}
		
		message << ".";
		
		if (killer && killer->get_team() == 'A') {
			display_message(message.str(), BLUE_COLOR, BLUE_SHADOW, bold);
		} else if (killer && killer->get_team() == 'B') {
			display_message(message.str(), RED_COLOR, RED_SHADOW, bold);
		} else {
			display_message(message.str());
		}
	}
	
	// If we were killed, freeze
	if (dead_player_id == m_player_id) {
		if (time_to_unfreeze) {
			freeze(time_to_unfreeze);
		} else {
			m_players[m_player_id].reset_energy();
			update_energy_bar();
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
			display_message(message, BLUE_COLOR, BLUE_SHADOW, true);
		} else {
			display_message(message, RED_COLOR, RED_SHADOW, true);
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
	
	// Uncomment this code to return gate raise/lower messages.
	/*ostringstream message;
	
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
	}*/
	
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
 * When a new round packet is received.
 */
void GameController::new_round(PacketReader& reader) {
	if (m_players.empty()) {
		// WELCOME packet not received yet
		// do NOT send an ACK for this NEW_ROUND packet, so that the server will resend it, hopefully after the WELCOME has come in.
		return;
	}

	/*
	 * Process the packet
	 */
	string 		map_name;
	int 		map_revision;
	bool		game_started;
	uint64_t	time_until_start;
	reader >> map_name >> map_revision >> game_started >> time_until_start;

	send_ack(reader);

	/*
	 * Tell the player what's going on
	 */
	ostringstream	message;
	if (game_started) {
		if (m_game_state == GAME_OVER) {
			m_game_state = GAME_IN_PROGRESS;
		}
		if (time_until_start == numeric_limits<uint64_t>::max()) {
			message << "Game in progress on map " << map_name << ". You will spawn when the next game starts.";
		} else if (time_until_start/1000 > 0) {
			message << "Game in progress on map " << map_name << ". " << time_until_start / 1000 << " seconds until spawn.";
		}
	} else if (time_until_start/1000 > 0) {
		message << "Game starts in " << time_until_start/1000 << " seconds on map " << map_name << ".";
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

	/*
	 * Reset the game state (TODO: add more stuff here)
	 */
	m_round_end_time = 0;
	clear_weapons();
	m_last_damage_time = 0;
	m_last_recharge_time = 0;
	m_last_weapon_switch = 0;
}

/*
 * Called when the game stop packet is received.
 */
void GameController::round_over(PacketReader& reader) {
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
	m_players[m_player_id].set_is_grabbing_obstacle(false);
	m_time_to_unfreeze = 0;
	m_total_time_frozen = 0;
	update_energy_bar(0);
	reset_weapons();
}

void GameController::round_start(PacketReader& reader) {
	// ACK TODO: Only accept this packet if we have gotten a related new_round packet

	if (m_game_state == GAME_OVER) {
		m_game_state = GAME_IN_PROGRESS;
	}

	uint64_t		time_left_in_game;
	reader >> time_left_in_game;

	if (time_left_in_game == numeric_limits<uint64_t>::max()) {
		m_round_end_time = 0;
	} else {
		m_round_end_time = get_ticks() + time_left_in_game;
	}

	toggle_score_overlay(false);
	m_sound_controller->play_sound("begin");
	display_message("Game started!");

	send_ack(reader);
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

		if (player->get_team() == 'A') {
			display_message(msg.str().c_str(), BLUE_COLOR, BLUE_SHADOW);
		} else {
			display_message(msg.str().c_str(), RED_COLOR, RED_SHADOW);
		}

		// Re-create the name sprite.
		recreate_name(player);
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
 * Recreate the player name sprite.
 */
void GameController::recreate_name(GraphicalPlayer* player) {
	// Re-create the name sprite.
	if (player->get_team() == 'A') {
		m_text_manager->set_active_color(BLUE_COLOR);
		m_text_manager->set_shadow_color(BLUE_SHADOW);
	} else {
		m_text_manager->set_active_color(RED_COLOR);
		m_text_manager->set_shadow_color(RED_SHADOW);
	}
	m_text_manager->remove_string(player->get_name_sprite());
	m_text_manager->set_active_font(m_font);
	string name_string = player->get_name();
	if (player->is_frozen()) {
		m_font->set_font_style(false, true);
		name_string = "[" + name_string + "]";
	}
	player->set_name_sprite(m_text_manager->place_string(name_string, player->get_x(), player->get_y()-(player->get_radius()+30), TextManager::CENTER, TextManager::LAYER_MAIN));
	m_font->set_font_style(false, false);		
	m_text_manager->set_shadow_color(TEXT_SHADOW);
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
void GameController::display_message(string message, Color color, Color shadow, bool bold) {
	m_text_manager->set_active_color(color);
	m_text_manager->set_shadow_color(shadow);
	if (bold) {
		m_text_manager->set_active_font(m_bold_font);
	} else {
		m_text_manager->set_active_font(m_font);
	}
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
		uint64_t	time_left_in_game;
		string		server_name;
		string		server_location;
		info_packet >> server_protocol_version >> current_map_name >> team_count[0] >> team_count[1] >> max_players >> uptime >> time_left_in_game >> server_name >> server_location;
		
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

	if (m_version_nag1 != NULL) {
		m_main_menu.remove_item(m_version_nag1);
		delete m_version_nag1;
		m_version_nag1 = NULL;
	}
	if (m_version_nag2 != NULL) {
		m_main_menu.remove_item(m_version_nag2);
		delete m_version_nag2;
		m_version_nag2 = NULL;
	}
	m_text_manager->set_active_font(m_menu_font);
	m_text_manager->set_active_color(TEXT_COLOR);
	Text *nag1 = m_text_manager->render_string("There is an upgrade available!", m_window->get_width() - 30, 200, TextManager::RIGHT);
	Text *nag2 = m_text_manager->render_string(message.str(), m_window->get_width() - 30, 240, TextManager::RIGHT);
	m_version_nag1 = new TextMenuItem(nag1, "", MenuItem::STATIC);
	m_version_nag2 = new TextMenuItem(nag2, "", MenuItem::STATIC);
	m_main_menu.add_item(m_version_nag1);
	m_main_menu.add_item(m_version_nag2);
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
	if (!m_params.process_param_packet(packet)) {
		// Parameter not recognized - reject the packet
		return;
	}

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

void GameController::change_weapon(const char* weapon_name) {
	if (Weapon* weapon = get_weapon(weapon_name)) {
		change_weapon(weapon);
	}
}

void GameController::change_weapon(unsigned int n) {
	if (n >= m_weapons.size()) {
		return;
	}

	map<string, Weapon*>::iterator it(m_weapons.begin());
	advance(it, n);
	change_weapon(it->second);
}

void	GameController::change_weapon(Weapon* weapon) {
	if (GraphicalPlayer* player = get_player_by_id(m_player_id)) {
		if (m_current_weapon != weapon && !player->is_frozen() && !player->is_invisible() && !player->is_dead()) {
			m_current_weapon = weapon;
			m_last_weapon_switch = get_ticks();
			update_curr_weapon_image();
			m_current_weapon->select(*player, *this);
		}
	}
}

double	GameController::get_crosshairs_angle() const {
	if (m_players.empty()) {
		return 0;
	}

	Point	crosshairs_location(m_crosshairs->get_x() + m_offset_x, m_crosshairs->get_y() + m_offset_y);
	return Vector(crosshairs_location - get_player_by_id(m_player_id)->get_position()).get_angle();
}

void	GameController::freeze(uint64_t time_to_unfreeze) {
	if (GraphicalPlayer* player = get_player_by_id(m_player_id)) {
		if (!player->is_frozen() && time_to_unfreeze != 0) {
			m_frozen_status_rect->set_y(m_screen_height/2 + player->get_radius() + 15);
			m_frozen_status_rect_back->set_y(m_frozen_status_rect->get_y());
			m_frozen_status_text->set_y(m_frozen_status_rect->get_y());
			m_sound_controller->play_sound("freeze");
			player->set_is_frozen(true);
			if (m_radar->get_mode() == RADAR_ON) {
				m_radar->set_blip_alpha(m_player_id, 0.5);
			}
			recreate_name(player);
			m_time_to_unfreeze = get_ticks() + time_to_unfreeze;
			m_total_time_frozen = time_to_unfreeze;
		}
	}
}

void	GameController::send_packet(PacketWriter& packet) {
	m_network.send_packet(packet);
}

void	GameController::play_sound(const char* sound_name) {
	m_sound_controller->play_sound(sound_name);
}

Weapon*	GameController::get_weapon(const string& name) {
	map<string, Weapon*>::iterator it(m_weapons.find(name));
	return it == m_weapons.end() ? NULL : it->second;
}

void	GameController::update_curr_weapon_image() {
	if (m_current_weapon == NULL) {
		if (m_curr_weapon_image != NULL) {
			m_curr_weapon_image->set_invisible(true);
		}
		return;
	}
	m_window->unregister_hud_graphic(m_curr_weapon_image);
	delete m_curr_weapon_image;
	m_curr_weapon_image = m_graphics_cache.new_graphic<Sprite>(m_path_manager.data_path(m_current_weapon->hud_graphic(), "sprites"));
	m_curr_weapon_image->set_x(m_cooldown_bar->get_x());
	m_curr_weapon_image->set_y(m_cooldown_bar->get_y() - m_curr_weapon_image->get_image_height()/2 - 5);
	m_curr_weapon_image->set_invisible(false);
	m_window->register_hud_graphic(m_curr_weapon_image);
}

void	GameController::reset_weapons() {
	for (map<string, Weapon*>::iterator it(m_weapons.begin()); it != m_weapons.end(); ++it) {
		it->second->reset();
	}
}

void	GameController::clear_weapons() {
	for (map<string, Weapon*>::iterator it(m_weapons.begin()); it != m_weapons.end(); ++it) {
		delete it->second;
	}
	m_weapons.clear();
}

void	GameController::activate_radar_blip(const Player& player) {
	m_radar->activate_blip(player.get_id(), get_ticks(), m_params.radar_blip_duration);
}

bool	GameController::damage (int amount, const Player* aggressor) {
	if (amount == 0 || m_players.empty() || m_players[m_player_id].is_frozen() || m_players[m_player_id].is_dead()) {
		// No effect on already frozen players
		return false;
	}

	m_players[m_player_id].change_energy(-amount);
	m_last_damage_time = get_ticks();
	update_energy_bar();
	if (m_players[m_player_id].is_dead()) {
		// Inform the server that we died
		PacketWriter		died_packet(PLAYER_DIED_PACKET);
		died_packet << m_player_id << (aggressor ? aggressor->get_id() : 0);
		m_network.send_packet(died_packet);

		return true;
	}
	return false;
}

GameController::HitObject::HitObject (double arg_distance, Point arg_point, BaseMapObject* arg_map_object)
{
	distance = arg_distance;
	point = arg_point;
	map_object = arg_map_object;
	player = 0;
}

GameController::HitObject::HitObject (double arg_distance, Point arg_point, Player* arg_player)
{
	distance = arg_distance;
	point = arg_point;
	map_object = 0;
	player = arg_player;
}

// Starting from the given point, shoot in a STRAIGHT LINE in the given direction,
// and populate the given set with the objects that are hit.
void	GameController::shoot_in_line(Point startpos, double direction, multiset<HitObject>& hit_objects)
{
	Point	endpos(startpos + Vector::make_from_magnitude(m_map_width + m_map_height, direction));

	//
	// First, find what map objects this line hits
	//
	const list<BaseMapObject*>& map_objects(m_map->get_objects());
	for (list<BaseMapObject*>::const_iterator it(map_objects.begin()); it != map_objects.end(); ++it) {
		BaseMapObject*	thisobj = *it;
		if (!thisobj->is_shootable() || !thisobj->is_intersectable()) {
			continue;
		}

		Point intersection = thisobj->get_bounding_shape()->intersects_line(startpos, endpos, NULL);
		if (intersection.x == -1 && intersection.y == -1) {
			// Not intersecting line
			continue;
		}
		
		double distance = Point::distance(startpos, intersection);
		if (distance != -1) {
			hit_objects.insert(HitObject(distance, intersection, thisobj));
		}
	}
	
	//
	// Now, find what players this line hits
	//
	for (map<uint32_t, GraphicalPlayer>::iterator it(m_players.begin()); it != m_players.end(); ++it) {
		GraphicalPlayer& thisplayer = it->second;
		if (thisplayer.get_id() == m_player_id) {
			continue;
		}

		Circle	player_circle(thisplayer.get_position(), thisplayer.get_radius());

		Point	intersection = player_circle.intersects_line(startpos, endpos, NULL);
		if (intersection.x == -1 && intersection.y == -1) {
			// Not intersecting line
			continue;
		}
		
		double	distance = Point::distance(startpos, intersection);
		if (distance != -1) {
			hit_objects.insert(HitObject(distance, intersection, &thisplayer));
		}
	}
	
	//
	// Finally, find the nearest map edge where the shot will hit.
	//
	Point	edge_intersection = m_map_polygon.intersects_line(startpos, endpos, NULL);

	if (edge_intersection.x != -1 || edge_intersection.y != -1) {		
		double	distance = Point::distance(startpos, edge_intersection);
		if (distance != -1) {
			hit_objects.insert(HitObject(distance, edge_intersection));
		}
	}
}

// This is a COMPATIBILITY WRAPPER around the more general shoot_in_line() function above.
// Code should be migrated to use the new function.
Point GameController::find_shootable_object(Point startpos, double direction, BaseMapObject*& hit_map_object, Player*& hit_player) {
	multiset<HitObject>	hit_objects;
	shoot_in_line(startpos, direction, hit_objects);

	if (hit_objects.empty()) {
		return Point(-1, -1);
	}

	const HitObject&	nearest_hit(*hit_objects.begin());
	hit_map_object = nearest_hit.map_object;
	hit_player = nearest_hit.player;
	return nearest_hit.point;
}



void	GameController::weapon_select(PacketReader& reader)
{
	uint32_t		player_id;
	string			weapon_name;

	reader >> player_id >> weapon_name;

	GraphicalPlayer*	player = get_player_by_id(player_id);

	if (!player) {
		return;
	}

	if (Weapon* weapon = get_weapon(weapon_name)) {
		weapon->select(*player, *this);
	}
}

void	GameController::weapon_info_packet(PacketReader& reader)
{
	size_t			weapon_index;
	WeaponReader		weapon_data;
	reader >> weapon_index;
	reader >> weapon_data;

	if (!m_weapons.count(weapon_data.get_id())) {
		if (Weapon* weapon = Weapon::new_weapon(weapon_data)) {
			m_weapons.insert(std::make_pair(weapon->get_id(), weapon));
			send_ack(reader);
			if (weapon_index == 0) {
				m_current_weapon = weapon;
				update_curr_weapon_image();
				if (Player* curr_player = get_player_by_id(m_player_id)) {
					weapon->select(*curr_player, *this);
				}
			}
			init_weapon_selector();
		}
	}
}

void GameController::spawn_packet(PacketReader& reader) {
	// ACK TODO: ignore packet if we have already spawned, or if round has not started yet
	Point			position;
	Vector			velocity;
	bool			is_grabbing_obstacle;
	bool			is_alive;
	uint64_t		freeze_time;

	reader >> position >> velocity >> is_grabbing_obstacle >> is_alive >> freeze_time;
	send_ack(reader);

	GraphicalPlayer*	player = get_player_by_id(m_player_id);

	if (!player) {
		return;
	}

	player->set_position(position);
	player->set_velocity(velocity);
	player->set_is_grabbing_obstacle(is_grabbing_obstacle);

	if (is_alive) {
		player->reset_energy();
		player->set_is_invisible(false);
		player->set_is_frozen(false);
		if (freeze_time) {
			freeze(freeze_time);
		}
	} else {
		player->set_energy(0);
		player->set_is_invisible(true);
		player->set_is_frozen(true);
	}

	update_energy_bar();

	// TODO: Move all this code below into a helper function!

	// Update the radar and name sprite
	m_radar->move_blip(player->get_id(), player->get_x(), player->get_y());
	m_radar->set_blip_invisible(player->get_id(), player->is_invisible());
	player->get_name_sprite()->set_invisible(player->is_invisible());
	
	// If invisible or frozen, set these things appropriately and show/hide the sprite.
	if (player->is_invisible()) {
		player->set_velocity(0, 0);
	} else {
		// Reposition the name sprite to reflect the player's new position
		m_text_manager->reposition_string(player->get_name_sprite(), player->get_x(), player->get_y() - (player->get_radius()+30), TextManager::CENTER);
	}
	
	if (m_radar->get_mode() == RADAR_ON) {
		m_radar->set_blip_alpha(player->get_id(), player->is_frozen() ? 0.5 : 1.0);
	}
}

void GameController::populate_graphic_group(GraphicGroup& group, const char* str) {
	StringTokenizer	item_tokenizer(str, ';');
	while (item_tokenizer) {
		string	item_string;
		item_tokenizer >> item_string;

		Point	position;
		double	rotation;
		int	priority;
		string	sprite_name;

		StringTokenizer(item_string, ':') >> position >> rotation >> priority >> sprite_name;

		Sprite*	sprite = m_graphics_cache.new_graphic<Sprite>(m_path_manager.data_path(sprite_name.c_str(), "sprites"));
		sprite->set_x(position.x);
		sprite->set_y(position.y);
		sprite->set_rotation(rotation);
		sprite->set_priority(priority);

		group.add_graphic(sprite);
	}
}

void GameController::add_front_arm(GraphicGroup& group, const char* sprite_name) {
	Sprite*	sprite = m_graphics_cache.new_graphic<Sprite>(m_path_manager.data_path(sprite_name, "sprites"));
	sprite->set_x(0);
	sprite->set_y(0);
	sprite->set_rotation(0);
	sprite->set_priority(0);
	group.add_graphic(sprite);
}

void GameController::register_front_arm_graphic(Player& base_player, const char* normal_str, const char* firing_str) {
	if (GraphicalPlayer* player = get_player_by_id(base_player.get_id())) {
		make_front_arm_graphic(*player->get_sprite(), player->get_team() == 'A' ? "blue_frontarm.png" : "red_frontarm.png", normal_str, firing_str);
	}
}

void GameController::make_front_arm_graphic(GraphicGroup& player_sprite, const char* arm_sprite, const char* normal_str, const char* firing_str) {
	GraphicGroup	normal_group;
	GraphicGroup	firing_group;

	if (normal_str) {
		populate_graphic_group(normal_group, normal_str);
	}
	if (firing_str) {
		populate_graphic_group(firing_group, firing_str);
	}

	if (arm_sprite) {
		add_front_arm(normal_group, arm_sprite);
		add_front_arm(firing_group, arm_sprite);
	}

	firing_group.set_invisible(true);

	GraphicGroup	frontarm;
	frontarm.add_graphic(&normal_group, "normal");
	frontarm.add_graphic(&firing_group, "firing");
	frontarm.set_x(13);
	frontarm.set_y(-18);
	frontarm.set_center_x(13);
	frontarm.set_center_y(-18);

	player_sprite.remove_graphic("frontarm");
	player_sprite.add_graphic(&frontarm, "frontarm");
}

