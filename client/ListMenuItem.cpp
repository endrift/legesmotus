/*
 * client/ListMenuItem.cpp
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

#include "ListMenuItem.hpp"

using namespace LM;
using namespace std;

ListMenuItem::ListMenuItem(string name, MenuItem* reference) : MenuItem(name) {
	m_reference = reference;
	m_group.give_graphic(reference->get_graphic(), "reference");
	m_default = 0;
	m_current = 0;
}

ListMenuItem::~ListMenuItem() {
	// Prevent graphics from being deleted by the wrong object
	m_group.take_all_graphics();

	delete m_reference;
	for (vector<MenuItem*>::iterator iter = m_options.begin(); iter != m_options.end(); ++iter) {
		delete *iter;
	}
}

void ListMenuItem::state_changed(State old_state, State new_state) {
	m_reference->set_state(new_state);
	if (get_current_option() != NULL) {
		unsigned int index = get_current_index();
		if (old_state == CLICKED) {
			index = (index + 1) % size();
		}
		set_current_index(index);
		get_current_option()->set_state(new_state);
	}
}

string ListMenuItem::get_value() const {
	if (m_options.size() == 0) {
		return string();
	} else {
		return m_options[m_current]->get_name();
	}
}

string ListMenuItem::get_default_value() const {
	if (m_options.size() == 0) {
		return string();
	} else {
		return m_options[m_default]->get_name();
	}
}

const GraphicGroup* ListMenuItem::get_graphic() const {
	return &m_group;
}

GraphicGroup* ListMenuItem::get_graphic() {
	return &m_group;
}

bool ListMenuItem::is_mouse_over(int x, int y) const {
	return m_reference->is_mouse_over(x, y) || get_current_option()->is_mouse_over(x, y);
}

void ListMenuItem::add_option(MenuItem* option) {
	m_options.push_back(option);
	m_group.give_graphic(option->get_graphic());
	option->get_graphic()->set_invisible(true);
	if (m_options.size() == 1) {
		set_current_index(0);
		set_default_index(0);
	}
}

void ListMenuItem::remove_option(MenuItem* option) {
	for (vector<MenuItem*>::iterator iter = m_options.begin(); iter != m_options.end();) {
		if (*iter == option) {
			m_group.take_graphic((*iter)->get_graphic());
			(*iter)->get_graphic()->set_invisible(true);
			delete *iter;
			iter = m_options.erase(iter);
			if (m_options.size() >= m_current) {
				set_current_index(m_options.size() - 1);
			}
			if (m_options.size() >= m_default) {
				set_default_index(m_options.size() - 1);
			}
		} else {
			++iter;
		}
	}
}

const MenuItem* ListMenuItem::get_current_option() const {
	if (m_current < m_options.size()) {
		return m_options[m_current];
	}
	return NULL;
}

MenuItem* ListMenuItem::get_current_option() {
	if (m_current < m_options.size()) {
		return m_options[m_current];
	}
	return NULL;
}

const MenuItem* ListMenuItem::get_default_option() const {
	if (m_default < m_options.size()) {
		return m_options[m_default];
	}
	return NULL;
}

MenuItem* ListMenuItem::get_default_option() {
	if (m_default < m_options.size()) {
		return m_options[m_default];
	}
	return NULL;
}

const MenuItem* ListMenuItem::get_option(unsigned int index) const {
	if (index < m_options.size()) {
		return m_options[index];
	}
	return NULL;
}

MenuItem* ListMenuItem::get_option(unsigned int index) {
	if (index < m_options.size()) {
		return m_options[index];
	}
	return NULL;
}

unsigned int ListMenuItem::size() const {
	return m_options.size();
}

void ListMenuItem::set_current_index(unsigned int index) {
	if (m_current < m_options.size()) {
		m_options[m_current]->get_graphic()->set_invisible(true);
	}
	if (index >= m_options.size()) {
		index = m_options.size() - 1;
	}
	m_current = index;
	m_options[m_current]->get_graphic()->set_invisible(false);
	m_options[m_current]->set_state(get_state());
}

void ListMenuItem::set_default_index(unsigned int index) {
	if (index >= m_options.size()) {
		m_default = m_options.size() - 1;
	} else {
		m_default = index;
	}
}

unsigned int ListMenuItem::get_current_index() const {
	return m_current;
}

unsigned int ListMenuItem::get_default_index() const {
	return m_default;
}

void ListMenuItem::reset() {
	set_current_index(m_default);
}
