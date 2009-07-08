/*
 * client/ChatLog.cpp
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

#include "ChatLog.hpp"
#include "ServerBrowser.hpp"
#include "GameController.hpp"
#include "GameWindow.hpp"
#include "TextManager.hpp"
#include "common/network.hpp"
#include "common/misc.hpp"

using namespace LM;
using namespace std;

const int ChatLog::TEXT_LAYER = -7;
const int ChatLog::LINE_SPACING = 15;
const int ChatLog::RIGHT_PADDING = 25;

ChatLog::ChatLog(GameController& parent, GameWindow* window, TextManager* textmanager, int screenwidth, 
				int screenheight, Font* standardfont, Font* mediumfont, Font* menufont) : m_parent(parent) {
	m_window = window;
	m_text_manager = textmanager;
	m_font = standardfont;
	m_medium_font = mediumfont;
	m_menu_font = menufont;
	m_screen_width = screenwidth;
	m_screen_height = screenheight;
	m_is_invisible = false;

	m_background = new TableBackground(2, m_screen_width * .70);
	m_background->set_row_height(0, 43);
	m_background->set_row_height(1, m_screen_height * .70 - m_background->get_row_height(0));
	m_background->set_priority(-5);
	m_background->set_border_color(Color(1,1,1,0.8));
	m_background->set_border_width(2);
	m_background->set_cell_color(0, Color(0.2,0.1,0.1,0.8));
	m_background->set_cell_color(1, Color(0.0,0.0,0.0,0.9));
	m_background->set_y(m_screen_height * .15);
	m_background->set_x(m_screen_width/2);
	m_background->set_border_collapse(true);
	m_background->set_corner_radius(20);
	m_window->register_hud_graphic(m_background);
	
	m_scrollbar = new ScrollBar();
	m_scrollbar->set_priority(-6);
	m_scrollbar->set_height(m_background->get_image_height() - m_background->get_row_height(0) - 20);
	m_scrollbar->set_x(m_background->get_x() + m_background->get_image_width()/2 - 20);
	m_scrollbar->set_y(m_background->get_y() + m_background->get_row_height(0) + 5 + m_scrollbar->get_height()/2);
	m_scrollbar->set_section_color(ScrollBar::BUTTONS, Color(0.7,0.2,0.1));
	m_scrollbar->set_section_color(ScrollBar::TRACK, Color(0.2,0.1,0.1));
	m_scrollbar->set_section_color(ScrollBar::TRACKER, Color(0.2,0.2,0.4));
	m_scrollbar->set_scroll_speed(3);
	
	m_scrollarea = new ScrollArea(m_background->get_x() - m_background->get_image_width()/2 - m_scrollbar->get_x() - m_scrollbar->get_image_width() + RIGHT_PADDING, m_background->get_image_height() - m_background->get_row_height(0) - 30,10,m_scrollbar);
	m_scrollarea->set_priority(TEXT_LAYER);
	m_scrollarea->get_group()->set_priority(TEXT_LAYER);
	m_scrollarea->set_x(m_background->get_x() - RIGHT_PADDING);
	m_scrollarea->set_y(m_background->get_y() + m_background->get_row_height(0) + 15);
	m_scrollarea->set_center_x(m_scrollarea->get_width()/2);
	m_scrollarea->set_center_y(0);
	
	m_window->register_hud_graphic(m_scrollbar);
	m_window->register_hud_graphic(m_scrollarea);
	
	m_title = m_text_manager->place_string("Chat Log", m_background->get_x(), m_background->get_y() + 10, TextManager::LEFT, TextManager::LAYER_HUD, TEXT_LAYER);
	m_title->set_x(m_background->get_x() - m_title->get_image_width()/2);

	m_scrollbar->scroll(1);
}

ChatLog::~ChatLog() {
	delete m_background;
	delete m_scrollbar;
	delete m_scrollarea;
}

void ChatLog::set_visible(bool visible) {
	m_is_invisible = !visible;
	
	m_background->set_invisible(!visible);
	m_title->set_invisible(!visible);
	
//	for (unsigned int i = 0; i < m_items.size(); i++) {
//		m_items[i]->set_invisible(!visible);
//	}
	
	m_scrollbar->set_invisible(!visible);
	m_scrollarea->set_invisible(!visible);
}

void ChatLog::add_message(string message, Color color) {
	double old_progress = m_scrollbar->get_scroll_progress();
	m_items.push_back(m_text_manager->place_string(message, m_background->get_x() - m_background->get_image_width()/2 - m_scrollarea->get_x() + m_scrollarea->get_width()/2 + 10, LINE_SPACING * m_items.size(), TextManager::LEFT, TextManager::LAYER_HUD));
	m_items[m_items.size()-1]->set_priority(TEXT_LAYER);
	m_window->unregister_hud_graphic(m_items[m_items.size()-1]);
	m_scrollarea->get_group()->add_graphic(m_items[m_items.size()-1], message);
	m_scrollarea->set_content_height(LINE_SPACING * (m_items.size()));
	if (old_progress == 1) {
		m_scrollbar->scroll(1);
	}
}

void ChatLog::delete_message(int num) {
}

void ChatLog::scrollbar_button_event(const SDL_MouseButtonEvent& event) {
	m_scrollbar->mouse_button_event(event);
}

void ChatLog::scrollbar_motion_event(const SDL_MouseMotionEvent& event) {
	m_scrollbar->mouse_motion_event(event);
}

void ChatLog::autoscroll(double scale) {
	m_scrollbar->autoscroll(scale);
}

bool ChatLog::is_invisible() {
	return m_is_invisible;
}
