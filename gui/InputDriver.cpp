/*
 * gui/InputDriver.cpp
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

#include "InputDriver.hpp"

using namespace LM;
using namespace std;

void InputDriver::register_event(const KeyEvent& event) {
	m_key_events.push(event);
}

void InputDriver::register_event(const MouseMotionEvent& event) {
	m_motion_events.push(event);
}

void InputDriver::register_event(const MouseButtonEvent& event) {
	m_button_events.push(event);
}

bool InputDriver::poll_keys(KeyEvent* event) {
	if (m_key_events.empty()) {
		return false;
	}

	*event = m_key_events.front();
	m_key_events.pop();
	return true;
}

bool InputDriver::poll_mouse_motion(MouseMotionEvent* event) {
	if (m_motion_events.empty()) {
		return false;
	}

	*event = m_motion_events.front();
	m_motion_events.pop();
	return true;
}

bool InputDriver::poll_mouse_buttons(MouseButtonEvent* event) {
	if (m_button_events.empty()) {
		return false;
	}

	*event = m_button_events.front();
	m_button_events.pop();
	return true;
}
