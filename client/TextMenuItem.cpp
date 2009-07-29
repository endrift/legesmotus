/*
 * client/TextMenuItem.cpp
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

#include "TextMenuItem.hpp"
#include "Text.hpp"
#include "Graphic.hpp"
#include "GameController.hpp"

using namespace LM;
using namespace std;

TextMenuItem::TextMenuItem(Text* text, string value, State state) : MenuItem(value, state),
m_plain_fg(GameController::TEXT_COLOR), m_plain_bg(GameController::TEXT_SHADOW),
m_hover_fg(GameController::BUTTON_HOVER_COLOR), m_hover_bg(GameController::BUTTON_HOVER_SHADOW),
m_disabled_fg(GameController::GREYED_COLOR), m_disabled_bg(GameController::GREYED_SHADOW) {
	m_text = text;
	state_changed(state, state);
}

void TextMenuItem::state_changed(State old_state, State new_state) {
	switch (new_state) {
	case NORMAL:
	case STATIC:
		m_text->set_color(m_plain_fg);
		m_text->set_shadow_color(m_plain_bg);
		break;
	case CLICKED:
		//TODO separate case
	case HOVER:
		m_text->set_color(m_hover_fg);
		m_text->set_shadow_color(m_hover_bg);
		break;
	case DISABLED:
		m_text->set_color(m_disabled_fg);
		m_text->set_shadow_color(m_disabled_bg);
		break;
	}
}

void TextMenuItem::set_plain_fg_color(const Color& color) {
	m_plain_fg = color;
}

void TextMenuItem::set_plain_bg_color(const Color& color) {
	m_plain_bg = color;
}

void TextMenuItem::set_hover_fg_color(const Color& color) {
	m_hover_fg = color;
}

void TextMenuItem::set_hover_bg_color(const Color& color) {
	m_hover_bg = color;
}

void TextMenuItem::set_disabled_fg_color(const Color& color) {
	m_disabled_fg = color;
}

void TextMenuItem::set_disabled_bg_color(const Color& color) {
	m_disabled_bg = color;
}

const Graphic* TextMenuItem::get_graphic() const {
	return m_text;
}

Graphic* TextMenuItem::get_graphic() {
	return m_text;
}
