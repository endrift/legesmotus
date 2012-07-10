/*
 * gui/Button.cpp
 *
 * This file is part of Leges Motus, a networked, 2D shooter set in zero gravity.
 * 
 * Copyright 2009-2011 Andrew Ayer, Nathan Partlan, Jeffrey Pfau
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

#include "Button.hpp"
#include "Label.hpp"

using namespace LM;
using namespace std;

Button::Button(Widget* parent) : BackgroundFrame(parent) {
	m_caption = NULL;
	m_callback_object = NULL;
	m_caption_string = "";
	m_is_hovered = false;
	m_change_on_hover = false;
	m_change_on_mousedown = false;
	
	set_state(STATE_NORMAL);
}

Button::~Button() {
	delete m_caption;
}

void Button::set_label(string caption, Font* font) {
	delete m_caption;
	
	m_caption_string = caption;
	m_caption = new Label(caption, font, this);
	m_caption->set_align(Label::ALIGN_CENTER);
	m_caption->set_y(m_caption->get_height()/4.0);
}

void Button::set_callback(ButtonCallback* callback) {
	m_callback_object = callback;
}

void Button::private_mouse_clicked(bool child_handled, float x, float y, bool down, int button) {
	if (m_callback_object != NULL) {
		if (down) {
			if (m_state != STATE_INACTIVE) {
				set_state(STATE_PRESSED);
				m_callback_object->button_pressed(get_id(), m_caption_string);
			}
		} else {
			if (m_state == STATE_PRESSED) {
				if (contains_point(x, y)) {
					m_callback_object->button_released(get_id(), m_caption_string);
				}
				set_state(STATE_NORMAL);
			}
		}
	}
}

void Button::private_mouse_moved(bool child_handled, float x, float y, float delta_x, float delta_y) {
	m_is_hovered = contains_point(x, y);
	update_colors();
}

void Button::set_normal_color(const Color& c, ColorType type) {
	ASSERT((unsigned long) type < COLOR_MAX);
	m_normal_colors[type] = c;
	update_colors();
}

const Color& Button::get_normal_color(ColorType type) const {
	ASSERT((unsigned long) type < COLOR_MAX);
	return m_normal_colors[type];
}

void Button::set_inactive_color(const Color& c, ColorType type) {
	ASSERT((unsigned long) type < COLOR_MAX);
	m_inactive_colors[type] = c;
	update_colors();
}

const Color& Button::get_inactive_color(ColorType type) const {
	ASSERT((unsigned long) type < COLOR_MAX);
	return m_inactive_colors[type];
}

void Button::set_hover_color(const Color& c, ColorType type) {
	ASSERT((unsigned long) type < COLOR_MAX);
	m_hover_colors[type] = c;
	update_colors();
}

const Color& Button::get_hover_color(ColorType type) const {
	ASSERT((unsigned long) type < COLOR_MAX);
	return m_hover_colors[type];
}

void Button::set_pressed_color(const Color& c, ColorType type) {
	ASSERT((unsigned long) type < COLOR_MAX);
	m_pressed_colors[type] = c;
	update_colors();
}

const Color& Button::get_pressed_color(ColorType type) const {
	ASSERT((unsigned long) type < COLOR_MAX);
	return m_pressed_colors[type];
}

void Button::update_colors() {
	if (m_is_hovered && m_state == STATE_NORMAL && m_change_on_hover) {
		set_color(m_hover_colors[COLOR_PRIMARY], COLOR_PRIMARY);
		set_color(m_hover_colors[COLOR_SECONDARY], COLOR_SECONDARY);
	} else if (m_state == STATE_INACTIVE) {
		set_color(m_inactive_colors[COLOR_PRIMARY], COLOR_PRIMARY);
		set_color(m_inactive_colors[COLOR_SECONDARY], COLOR_SECONDARY);
	} else if (m_state == STATE_PRESSED && m_change_on_mousedown) {
		set_color(m_pressed_colors[COLOR_PRIMARY], COLOR_PRIMARY);
		set_color(m_pressed_colors[COLOR_SECONDARY], COLOR_SECONDARY);
	} else {
		set_color(m_normal_colors[COLOR_PRIMARY], COLOR_PRIMARY);
		set_color(m_normal_colors[COLOR_SECONDARY], COLOR_SECONDARY);
	}
}

void Button::set_state(ButtonState state) {
	m_state = state;
	
	update_colors();
}

Button::ButtonState Button::get_state() const {
	return m_state;
}

void Button::set_change_on_hover(bool change) {
	m_change_on_hover = change;
}

void Button::set_change_on_pressed(bool change) {
	m_change_on_mousedown = change;
}
