/*
 * gui/HumanController.cpp
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

#include "HumanController.hpp"
#include "InputDriver.hpp"
#include <cmath>

using namespace LM;
using namespace std;

HumanController::HumanController() {
	m_changeset = 0;
}

void HumanController::process_control(const Bindings::ControlEvent& event) {
	switch (event.type) {
	case Bindings::CONTROL_FIRE:
		m_changes[m_changeset ^ 1] |= FIRE_WEAPON;
		break;
	case Bindings::CONTROL_STOP_FIRE:
		m_changes[m_changeset ^ 1] |= STOP_FIRE_WEAPON;
		break;
	case Bindings::CONTROL_JUMP:
		m_changes[m_changeset ^ 1] |= JUMPING;
		break;
	case Bindings::CONTROL_BEGIN_TYPING:
		m_typing_message = true;
		break;
	default:
		// No default action
		break;
	}
}

void HumanController::set_viewport_size(int w, int h) {
	m_view_w = w;
	m_view_h = h;
}

void HumanController::key_pressed(const KeyEvent& event) {
	if (m_typing_message) {
		switch (event.type) {
		case KEY_LETTER:
			// TODO typing
			break;
		case KEY_ENTER:
			m_changes[m_changeset ^ 1] |= SEND_MESSAGE;
			m_typing_message = false;
			break;
		default:
			// TODO
			break;
		}
	} else {
		Bindings::ControlEvent e = m_bindings.process_event(event);
		process_control(e);
	}
}

void HumanController::mouse_moved(const MouseMotionEvent& event) {
	m_changes[m_changeset ^ 1] |= CHANGE_AIM;
	m_mouse_x = event.x;
	m_mouse_y = event.y;
}

void HumanController::mouse_clicked(const MouseButtonEvent& event) {
	Bindings::ControlEvent e = m_bindings.process_event(event);
	process_control(e);
}

void HumanController::system_event(const SystemEvent& event) {
}

void HumanController::update(uint64_t diff, const GameLogic& state) {
	m_changes[m_changeset] = 0;
	m_changeset ^= 1;
}

int HumanController::get_changes() const {
	return m_changes[m_changeset];
}

float HumanController::get_aim() const {
	int xrel = m_mouse_x - (m_view_w>>1);
	int yrel = m_mouse_y - (m_view_h>>1);

	return atan2(yrel, xrel);
}

float HumanController::get_distance() const {
	int xrel = m_mouse_x - (m_view_w>>1);
	int yrel = m_mouse_y - (m_view_h>>1);

	return hypot(xrel, yrel);
}

int HumanController::get_weapon() const {
	return m_weapon;
}

wstring HumanController::get_message() const {
	return L"";
}

void HumanController::received_message(const Player* p, const wstring& message) {
	// Nothing to do
}
