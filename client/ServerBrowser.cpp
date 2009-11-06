/*
 * client/ServerBrowser.cpp
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

#include "ServerBrowser.hpp"
#include "GameController.hpp"
#include "GameWindow.hpp"
#include "TextManager.hpp"
#include "common/network.hpp"
#include "common/misc.hpp"

using namespace LM;
using namespace std;

const int ServerBrowser::TEXT_LAYER = -4;

ServerBrowser::ServerBrowser(GameController& parent, GameWindow* window, TextManager* textmanager, int screenwidth, 
				int screenheight, Font* standardfont, Font* mediumfont, Font* menufont) : m_parent(parent) {
	m_window = window;
	m_text_manager = textmanager;
	m_font = standardfont;
	m_medium_font = mediumfont;
	m_menu_font = menufont;
	m_screen_width = screenwidth;
	m_screen_height = screenheight;
	
	m_selected_item = -1;
	
	m_background = new TableBackground(2, m_screen_width - 50);
	m_background->set_row_height(0, 43);
	m_background->set_row_height(1, m_screen_height - 203);
	m_background->set_priority(-2);
	m_background->set_border_color(Color(1,1,1,0.8));
	m_background->set_border_width(2);
	m_background->set_cell_color(0, Color(0.2,0.1,0.1,0.8));
	m_background->set_cell_color(1, Color(0.1,0.1,0.15,0.8));
	m_background->set_y(100);
	m_background->set_x(m_screen_width/2);
	m_background->set_border_collapse(true);
	m_background->set_corner_radius(20);
	m_window->register_hud_graphic(m_background);
	
	m_scrollbar = new ScrollBar();
	m_scrollbar->set_priority(-3);
	m_scrollbar->set_height(m_background->get_image_height() - m_background->get_row_height(0) - 20);
	m_scrollbar->set_x(m_background->get_x() + m_background->get_image_width()/2 - 20);
	m_scrollbar->set_y(m_background->get_y() + m_background->get_row_height(0) + 5 + m_scrollbar->get_height()/2);
	m_scrollbar->set_section_color(ScrollBar::BUTTONS, Color(0.7,0.2,0.1));
	m_scrollbar->set_section_color(ScrollBar::TRACK, Color(0.2,0.1,0.1));
	m_scrollbar->set_section_color(ScrollBar::TRACKER, Color(0.2,0.2,0.4));
	m_scrollbar->set_scroll_speed(3);
	
	m_scrollarea = new ScrollArea(m_background->get_image_width(), m_background->get_image_height() - m_background->get_row_height(0) - 30, m_background->get_image_width(), 10, NULL, m_scrollbar);
	m_scrollarea->set_priority(TEXT_LAYER);
	m_scrollarea->get_group()->set_priority(TEXT_LAYER);
	m_scrollarea->set_x(m_background->get_x() + 5);
	m_scrollarea->set_y(m_background->get_y() + m_background->get_row_height(0) + 15);
	m_scrollarea->set_center_x(m_scrollarea->get_width()/2);
	m_scrollarea->set_center_y(0);
	
	m_window->register_hud_graphic(m_scrollbar);
	m_window->register_hud_graphic(m_scrollarea);
	
	m_selection = new TableBackground(1, m_scrollbar->get_x() - (m_background->get_x() - m_background->get_image_width()/2) - 21);
	m_selection->set_row_height(0, 25);
	m_selection->set_priority(-3);
	m_selection->set_cell_color(0, Color(0.0, 0.0, 0.0, 0.8));
	m_selection->set_x(m_background->get_x() - 15);
	m_selection->set_y(145);
	m_window->register_hud_graphic(m_selection);
	
	for (int i = 0; i < 3; i++) {
		m_buttons.push_back(new TableBackground(1, 100));
		m_buttons[i]->set_row_height(0, 40);
		m_buttons[i]->set_priority(-1);
		m_buttons[i]->set_border_width(2);
		m_buttons[i]->set_border_color(Color(1,1,1,0.8));
		m_buttons[i]->set_cell_color(0, Color(0.1,0.1,0.15,0.8));
		m_buttons[i]->set_x(100 + 100 * i);
		m_buttons[i]->set_y(m_screen_height - m_buttons[i]->get_image_height() - 22);
		m_window->register_hud_graphic(m_buttons[i]);
	}
	
	m_text_manager->set_active_font(m_medium_font);
	
	m_items["namelabel"] = m_text_manager->place_string("Name", m_background->get_x() - m_background->get_image_width()/2 + 10, 110, TextManager::LEFT, TextManager::LAYER_HUD, TEXT_LAYER);
	m_items["maplabel"] = m_text_manager->place_string("Map", m_background->get_x(), 110, TextManager::LEFT, TextManager::LAYER_HUD, TEXT_LAYER);
	m_items["uptimelabel"] = m_text_manager->place_string("Uptime", m_background->get_x() + m_background->get_image_width()/7, 110, TextManager::LEFT, TextManager::LAYER_HUD, TEXT_LAYER);
	m_items["playerslabel"] = m_text_manager->place_string("Players", m_background->get_x() + m_background->get_image_width()/4, 110, TextManager::LEFT, TextManager::LAYER_HUD, TEXT_LAYER);
	m_items["pinglabel"] = m_text_manager->place_string("Ping", m_background->get_x() + m_background->get_image_width()/2 - 80, 110, TextManager::LEFT, TextManager::LAYER_HUD, TEXT_LAYER);
	m_items["backbutton"] = m_text_manager->place_string("Back", m_buttons[0]->get_x() - m_buttons[0]->get_image_width()/2 + 25, m_buttons[0]->get_y() + 8, TextManager::LEFT, TextManager::LAYER_HUD, TEXT_LAYER);
	m_items["refreshbutton"] = m_text_manager->place_string("Refresh", m_buttons[1]->get_x() - m_buttons[1]->get_image_width()/2 + 10, m_buttons[1]->get_y() + 8, TextManager::LEFT, TextManager::LAYER_HUD, TEXT_LAYER);
	m_items["connectbutton"] = m_text_manager->place_string("Connect", m_buttons[2]->get_x() - m_buttons[2]->get_image_width()/2 + 6, m_buttons[2]->get_y() + 8, TextManager::LEFT, TextManager::LAYER_HUD, TEXT_LAYER);
	
	m_server_list_count = 0;
}

ServerBrowser::~ServerBrowser() {
	this->clear();

	m_window->unregister_hud_graphic(m_background);
	delete m_background;
	m_window->unregister_hud_graphic(m_selection);
	delete m_selection;
	m_window->unregister_hud_graphic(m_scrollbar);
	delete m_scrollbar;
	m_window->unregister_hud_graphic(m_scrollarea);
	delete m_scrollarea;
	
	m_buttons.clear();
}

void ServerBrowser::set_visible(bool visible) {
	m_is_invisible = !visible;
	// If we're opening the browser, clear the list and re-scan.
	if (m_background->is_invisible() == visible && m_background->is_invisible() == true) {
		clear();
		m_parent.scan_all();
	}

	m_background->set_invisible(!visible);
	m_scrollbar->set_invisible(!visible);
	m_scrollarea->set_invisible(!visible);
	
	if (visible == false) {
		m_selection->set_invisible(!visible);
	}
	
	map<string, Text*>::iterator it;
	for ( it=m_items.begin() ; it != m_items.end(); it++ ) {
		Graphic* thisitem = (*it).second;
		thisitem->set_invisible(!visible);
	}
	
	for (unsigned int i = 0; i < m_buttons.size(); i++ ) {
		m_buttons[i]->set_invisible(!visible);
	}
}

void ServerBrowser::clear() {
	for (int num = 0; num < m_server_list_count; num++) {
		ostringstream printer;
		printer << "name";
		printer << num;
		if (m_items.find(printer.str()) == m_items.end()) {
			continue;
		}
		m_scrollarea->get_group()->remove_graphic(printer.str());
		m_items.erase(printer.str());
	
		printer.clear();
		printer << "map";
		printer << num;
		if (m_items.find(printer.str()) == m_items.end()) {
			continue;
		}
		m_scrollarea->get_group()->remove_graphic(printer.str());
		m_items.erase(printer.str());
	
		printer.clear();
		printer << "uptime";
		printer << num;
		if (m_items.find(printer.str()) == m_items.end()) {
			continue;
		}
		m_scrollarea->get_group()->remove_graphic(printer.str());
		m_items.erase(printer.str());
	
		printer.clear();
		printer << "players";
		printer << num;
		if (m_items.find(printer.str()) == m_items.end()) {
			continue;
		}
		m_scrollarea->get_group()->remove_graphic(printer.str());
		m_items.erase(printer.str());
	
		printer.clear();
		printer << "ping";
		printer << num;
		if (m_items.find(printer.str()) == m_items.end()) {
			continue;
		}
		m_scrollarea->get_group()->remove_graphic(printer.str());
		m_items.erase(printer.str());
	}
	
	m_server_list.clear();
	
	m_server_list_count = 0;
}

void ServerBrowser::add_entry(IPAddress server_address, const string& current_map_name, int team_count[2], int max_players, uint64_t uptime, uint64_t ping, const string& server_name, const string& server_location) {
	m_server_list.insert(m_server_list.begin() + m_server_list_count, server_address);
	
	m_text_manager->set_active_font(m_font);
	m_text_manager->set_active_color(1.0, 1.0, 1.0);
	
	ostringstream printer;
	printer << "name";
	printer << m_server_list_count;
	if (server_address.is_localhost()) {
		string hostname = "";
		uint16_t portno = 0;
		resolve_ip_address(hostname, &portno, server_address);
		ostringstream ipprinter;
		ipprinter << "localhost:" << portno;
		m_items[printer.str()] = m_text_manager->render_string(ipprinter.str(), m_items["namelabel"]->get_x() - m_scrollarea->get_x() + m_scrollarea->get_width()/2, 25 * m_server_list_count, TextManager::LEFT);
	} else if (server_name != "") {
		m_items[printer.str()] = m_text_manager->render_string(server_name, m_items["namelabel"]->get_x() - m_scrollarea->get_x() + m_scrollarea->get_width()/2, 25 * m_server_list_count, TextManager::LEFT);
	} else {
		m_items[printer.str()] = m_text_manager->render_string(format_ip_address(server_address, true), m_items["namelabel"]->get_x() - m_scrollarea->get_x() + m_scrollarea->get_width()/2, 25 * m_server_list_count, TextManager::LEFT);
	}
	m_items[printer.str()]->set_priority(TEXT_LAYER);
	m_scrollarea->get_group()->remove_graphic(printer.str());
	m_scrollarea->get_group()->add_graphic(m_items[printer.str()], printer.str());
	
	printer.clear();
	printer << "map";
	printer << m_server_list_count;
	m_items[printer.str()] = m_text_manager->render_string(current_map_name, m_items["maplabel"]->get_x() - m_scrollarea->get_x() + m_scrollarea->get_width()/2, 25 * m_server_list_count, TextManager::LEFT);
	m_items[printer.str()]->set_priority(TEXT_LAYER);
	m_scrollarea->get_group()->remove_graphic(printer.str());
	m_scrollarea->get_group()->add_graphic(m_items[printer.str()], printer.str());
	
	printer.clear();
	printer << "uptime";
	printer << m_server_list_count;
	
	string uptimestr = m_parent.format_time_from_millis(uptime);
	
	m_items[printer.str()] = m_text_manager->render_string(uptimestr, m_items["uptimelabel"]->get_x() - m_scrollarea->get_x() + m_scrollarea->get_width()/2, 25 * m_server_list_count, TextManager::LEFT);
	m_items[printer.str()]->set_priority(TEXT_LAYER);
	m_scrollarea->get_group()->remove_graphic(printer.str());
	m_scrollarea->get_group()->add_graphic(m_items[printer.str()], printer.str());
	
	printer.clear();
	printer << "players";
	printer << m_server_list_count;
	
	ostringstream playertotal;
	playertotal << (team_count[0] + team_count[1]);
	playertotal << "/";
	playertotal << max_players;
	m_items[printer.str()] = m_text_manager->render_string(playertotal.str(), m_items["playerslabel"]->get_x() - m_scrollarea->get_x() + m_scrollarea->get_width()/2, 25 * m_server_list_count, TextManager::LEFT);
	m_items[printer.str()]->set_priority(TEXT_LAYER);
	m_scrollarea->get_group()->remove_graphic(printer.str());
	m_scrollarea->get_group()->add_graphic(m_items[printer.str()], printer.str());
	
	printer.clear();
	printer << "ping";
	printer << m_server_list_count;
	
	ostringstream pingstr;
	
	pingstr << ping;
	m_items[printer.str()] = m_text_manager->render_string(pingstr.str(), m_items["pinglabel"]->get_x() - m_scrollarea->get_x() + m_scrollarea->get_width()/2, 25 * m_server_list_count, TextManager::LEFT);
	m_items[printer.str()]->set_priority(TEXT_LAYER);
	m_scrollarea->get_group()->remove_graphic(printer.str());
	m_scrollarea->get_group()->add_graphic(m_items[printer.str()], printer.str());

	m_scrollarea->set_content_height(25 * (m_server_list_count + 1));

	m_server_list_count++;
}

string ServerBrowser::check_button_press(int mouse_x, int mouse_y) {
	for (unsigned int i = 0; i < m_buttons.size(); i++) {
		TableBackground button = *m_buttons[i];
		if (mouse_x < button.get_x() - button.get_image_width()/2 ||
		    mouse_x > button.get_x() + button.get_image_width()/2 ||
		    mouse_y < button.get_y() ||
		    mouse_y > button.get_y() + button.get_image_height()) {
			 continue;
		}
		
		if (i == 0) {
			// Back.
			return "Back";
		} else if (i == 1) {
			// Refresh.
			return "Refresh";
		} else if (i == 2) {
			// Connect.
			return "Connect";
		}
	}
	return "";
}

int ServerBrowser::check_item_select(int mouse_x, int mouse_y) {
	int left_limit = int(m_background->get_x() - m_background->get_image_width()/2);
	int right_limit = int(m_scrollbar->get_x() - 15);			
	
	if (mouse_x < left_limit || mouse_x > right_limit) {
		m_selection->set_invisible(true);
		m_selected_item = -1;
		return -1;
	}
	
	if (mouse_y < m_scrollarea->get_y() || mouse_y > m_scrollarea->get_y() + m_scrollarea->get_height()) {
		m_selection->set_invisible(true);
		m_selected_item = -1;
		return -1;
	}
	
	int offset = int(m_scrollarea->get_y() - m_scrollarea->get_vert_scroll_progress_pixels());
	
	m_selected_item = int(((mouse_y-offset)/25) - m_items["name0"]->get_y() / 25);
	
	ostringstream itemname;
	itemname << "name" << m_selected_item;
	if (m_items.find(itemname.str()) == m_items.end()) {
		m_selection->set_invisible(true);
		m_selected_item = -1;
		return -1;
	}
	
	m_selection->set_y(m_items[itemname.str()]->get_y() + offset - 5);
	m_selection->set_invisible(false);
	
	// One last check to see if we're overlapping the edges too much.
	if (m_selection->get_y() < m_scrollarea->get_y() - 12 || 
		m_selection->get_y() + m_selection->get_image_height() >  
		m_scrollarea->get_y() + m_scrollarea->get_height() + 12) {
		
		m_selection->set_invisible(true);
		m_selected_item = -1;
		return -1;
	}
	
	return m_selected_item;
}

int ServerBrowser::get_selected_item() {
	return m_selected_item;
}

void ServerBrowser::scrollbar_button_event(const SDL_MouseButtonEvent& event) {
	m_scrollbar->mouse_button_event(event);
}

void ServerBrowser::scrollbar_motion_event(const SDL_MouseMotionEvent& event) {
	m_scrollbar->mouse_motion_event(event);
}

void ServerBrowser::autoscroll(double scale) {
	m_scrollbar->autoscroll(scale);
}

bool ServerBrowser::is_invisible() {
	return m_is_invisible;
}

int ServerBrowser::get_count() {
	return m_server_list_count;
}

void ServerBrowser::deselect() {
	m_selected_item = -1;
	m_selection->set_invisible(true);
}

IPAddress ServerBrowser::get_server_info(int num) {
	if (num > m_server_list_count) {
		return IPAddress();
	} else {
		return m_server_list[num];
	}
}

bool ServerBrowser::contains_ip(IPAddress ip) {
	for (unsigned int i = 0; i < m_server_list.size(); i++) {
		if (m_server_list[i] == ip) {
			return true;
		}
	}
	return false;
}
