/*
 * gui/TextInput.cpp
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

#include "TextInput.hpp"
#include "Label.hpp"

using namespace LM;
using namespace std;

TextInput::TextInput(Widget* parent) : BackgroundFrame(parent) {
	m_is_focused = false;
	m_mouse_down = false;
	m_callback_object = NULL;
	m_caption = NULL;
	m_cursor_blink_millis = 750.0f;
	m_cursor_transparency = 0.0f;
}

TextInput::~TextInput() {
	delete m_caption;
}

void TextInput::set_callback(TextInputCallback* callback) {
	m_callback_object = callback;
}

void TextInput::set_font(Font* font) {
	delete m_caption;
	
	m_caption = new Label(font, this);
	m_caption->set_align(Label::ALIGN_LEFT);
	m_caption->set_y(m_caption->get_height()/4.0);
	m_caption->set_x(-0.5 * get_width() + get_border_radius() + get_border_padding() + 1);
}

void TextInput::set_label(const wstring& caption) {
	m_caption_string = caption;
	if (m_caption != NULL) {
		m_caption->set_string(m_caption_string);
	}
}

void TextInput::private_mouse_clicked(bool child_handled, float x, float y, bool down, int button) {
	if (m_is_focused && !down && !contains_point(x, y)) {
		if (m_callback_object != NULL) {
			m_callback_object->input_completed(get_id(), this, m_caption_string);
		}
		m_is_focused = false;
		return;
	}
	
	if (!m_is_focused && contains_point(x, y)) {

		if (down) {
			m_mouse_down = true;
		} else if (m_mouse_down) {
			m_mouse_down = false;
			m_is_focused = true;
			
			if (m_callback_object != NULL) {
				m_callback_object->input_box_clicked(get_id(), this, m_caption_string);
			}
		}
	}
}

void TextInput::private_keypress(const KeyEvent& event) {
	if (event.down && event.type == KEY_LETTER) {
		set_label(m_caption_string + event.character);
		if (m_callback_object != NULL) {
			m_callback_object->input_changed(get_id(), this, m_caption_string);
		}
	} else if (event.down && event.type == KEY_BACKSPACE) {
		set_label(m_caption_string.substr(0, m_caption_string.length()-1));
		if (m_callback_object != NULL) {
			m_callback_object->input_changed(get_id(), this, m_caption_string);
		}
	} else if (event.down && event.type == KEY_ENTER) {
		if (m_callback_object != NULL) {
			m_callback_object->input_completed(get_id(), this, m_caption_string);
		}
		m_is_focused = false;
	}
}

void TextInput::set_cursor_color(const Color& color) {
	m_cursor_color = color;
}

void TextInput::set_cursor_blink_millis(uint64_t millis) {
	m_cursor_blink_millis = millis;
}

void TextInput::update(uint64_t timediff) {
	m_cursor_transparency += timediff * 1.0f / m_cursor_blink_millis;
	if (m_cursor_transparency > 2.0f) {
		m_cursor_transparency = m_cursor_transparency - 2.0f;
	}
	
	if (!m_is_focused) {
		m_cursor_transparency = 0.0f;
	}
	
	BackgroundFrame::update(timediff);
}

void TextInput::draw_extras(DrawContext* ctx) const {
	BackgroundFrame::draw_extras(ctx);
	
	ctx->push_transform();
	
	ctx->translate(1 + m_caption->get_width() + get_x() + m_caption->get_x(), get_y());
	
	Color cursor_color = m_cursor_color;
	
	if (m_cursor_transparency > 1.0f) {
		cursor_color.a = 2.0 - m_cursor_transparency;
	} else {
		cursor_color.a = m_cursor_transparency;
	}
	
	ctx->set_draw_color(cursor_color);
	
	ctx->draw_rect(1.0f, m_caption->get_height() * 0.9f);
	
	ctx->pop_transform();
}
