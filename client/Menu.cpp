/*
 * client/Menu.cpp
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

#include "Menu.hpp"

using namespace LM;
using namespace std;

Menu::Menu() {
	m_over = NULL;
	m_pressed = NULL;
}

Menu::~Menu() {
	for (vector<MenuItem*>::iterator iter = m_menu_items.begin(); iter != m_menu_items.end(); ++iter) {
		m_group.take_graphic((*iter)->get_graphic());
		delete *iter;
	}
}

void Menu::add_item_internal(MenuItem* item) {
	if (item == NULL) {
		return;
	}
	m_menu_items.push_back(item);
	m_group.give_graphic(item->get_graphic());
}

void Menu::remove_item_internal(MenuItem* item) {
	if (item == NULL) {
		return;
	}
	// TODO delete menu item here?
	for (vector<MenuItem*>::iterator iter = m_menu_items.begin(); iter != m_menu_items.end();) {
		if (*iter == item) {
			iter = m_menu_items.erase(iter);
		} else {
			++iter;
		}
	}
	m_group.take_graphic(item->get_graphic());
}

GraphicGroup* Menu::get_graphic_group() {
	return &m_group;
}

MenuItem* Menu::mouse_motion_event(const SDL_MouseMotionEvent& event) {
	MenuItem* seen_item = item_at_position(event.x, event.y);
	if (seen_item != m_over) {
		if (m_over != NULL && m_over->get_state() == MenuItem::HOVER) {
			m_over->set_state(MenuItem::NORMAL);
			mouseout(m_over, event.x, event.y);
		}
		if (seen_item != NULL && seen_item->get_state() == MenuItem::NORMAL) {
			seen_item->set_state(MenuItem::HOVER);
			mouseover(seen_item, event.x, event.y);
		}
		m_over = seen_item;
	}

	if (seen_item && seen_item->disabled()) {
		return NULL;
	}

	return seen_item;
}

MenuItem* Menu::mouse_button_event(const SDL_MouseButtonEvent& event) {
	if (event.button != SDL_BUTTON_LEFT) {
		return NULL;
	}
	if (event.state == SDL_PRESSED) {
		m_pressed = item_at_position(event.x, event.y);
		if (m_pressed != NULL && m_pressed->get_state() == MenuItem::HOVER) {
			m_pressed->set_state(MenuItem::CLICKED);
		}
		mousedown(m_pressed, event.x, event.y);
	} else {
		if (m_pressed != NULL && m_pressed->get_state() == MenuItem::CLICKED) {
			if (item_at_position(event.x, event.y) == m_pressed) {
				m_pressed->set_state(MenuItem::HOVER);
			} else {
				m_pressed->set_state(MenuItem::NORMAL);
			}
		}
		mouseup(m_pressed, event.x, event.y);
	}

	if (m_pressed && m_pressed->disabled()) {
		return NULL;
	}

	return m_pressed;
}
