/*
 * client/MenuItem.cpp
 *
 * This file is part of Leges Motus, a networked, 2D shooter set in zero gravity.
 * 
 * Copyright 2009-2010 Andrew Ayer, Nathan Partlan, Jeffrey Pfau
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

#include "MenuItem.hpp"
#include "Graphic.hpp"

using namespace LM;
using namespace std;

MenuItem::MenuItem(string name, State state) {
	m_name = name;
	m_state = state;
}

void MenuItem::set_state(State state) {
	State old_state = m_state;
	m_state = state;
	state_changed(old_state, state);
}

void MenuItem::set_name(string name) {
	m_name = name;
}

MenuItem::State MenuItem::get_state() const {
	return m_state;
}

string MenuItem::get_name() const {
	return m_name;
}

bool MenuItem::disabled() const {
	return m_state == DISABLED || m_state == STATIC;
}

bool MenuItem::is_mouse_over(int x, int y) const {
	return get_graphic()->is_over(x, y);
}
