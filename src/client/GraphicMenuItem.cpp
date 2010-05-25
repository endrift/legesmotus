/*
 * client/GraphicMenuItem.cpp
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
 * PARTICULAR PURPOSE.  See the full Graphic of the GNU General Public License for
 * further detail.
 * 
 * For a full copy of the GNU General Public License, please see the COPYING file
 * in the root of the source code tree.  You may also retrieve a copy from
 * <http://www.gnu.org/licenses/gpl-2.0.txt>, or request a copy by writing to the
 * Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 * 02111-1307  USA
 * 
 */

#include "GraphicMenuItem.hpp"


using namespace LM;
using namespace std;

const Color GraphicMenuItem::PLAIN_COLOR = Color(1.0, 1.0, 1.0, 0.6);
const Color GraphicMenuItem::HOVER_COLOR = Color(1.0, 1.0, 1.0, 1.0);
const Color GraphicMenuItem::DISABLED_COLOR = Color(0.8, 0.8, 0.8, 0.4);

GraphicMenuItem::GraphicMenuItem(Graphic* Graphic, string value, State state) : MenuItem(value, state),
m_plain(PLAIN_COLOR), m_hover(HOVER_COLOR), m_disabled(DISABLED_COLOR) {
	m_graphic = Graphic;
	m_hover_scale = 1.0;
	m_normal_scale = 1.0;
	state_changed(state, state);
}

void GraphicMenuItem::state_changed(State old_state, State new_state) {
	switch (new_state) {
	case NORMAL:
	case STATIC:
		m_graphic->set_color_intensity(m_plain);
		m_graphic->set_alpha(m_plain.a);
		m_graphic->set_scale_x(m_normal_scale);
		m_graphic->set_scale_y(m_normal_scale);
		break;
	case CLICKED:
		//TODO separate case
	case HOVER:
		m_graphic->set_color_intensity(m_hover);
		m_graphic->set_alpha(m_hover.a);
		m_graphic->set_scale_x(m_hover_scale);
		m_graphic->set_scale_y(m_hover_scale);
		break;
	case DISABLED:
		m_graphic->set_color_intensity(m_disabled);
		m_graphic->set_alpha(m_disabled.a);
		m_graphic->set_scale_x(m_normal_scale);
		m_graphic->set_scale_y(m_normal_scale);
		break;
	}
}

void GraphicMenuItem::set_plain_color(const Color& color) {
	m_plain = color;
	switch(get_state()) {
	case NORMAL:
	case STATIC:
		m_graphic->set_color_intensity(m_plain);
		m_graphic->set_alpha(m_plain.a);
		break;
	default:
		break;
	}
}

void GraphicMenuItem::set_hover_color(const Color& color) {
	m_hover = color;
	switch(get_state()) {
	case HOVER:
		m_graphic->set_color_intensity(m_hover);
		m_graphic->set_alpha(m_hover.a);
		break;
	default:
		break;
	}
}

void GraphicMenuItem::set_disabled_color(const Color& color) {
	m_disabled = color;
	switch(get_state()) {
	case DISABLED:
		m_graphic->set_color_intensity(m_disabled);
		m_graphic->set_alpha(m_disabled.a);
		break;
	default:
		break;
	}
}

void GraphicMenuItem::set_scale(double factor) {
	m_normal_scale = factor;
	if (get_state() != HOVER) {
		m_graphic->set_scale_x(m_normal_scale);
		m_graphic->set_scale_y(m_normal_scale);
	}
}

void GraphicMenuItem::set_hover_scale(double factor) {
	m_hover_scale = factor;
	if (get_state() == HOVER) {
		m_graphic->set_scale_x(m_hover_scale);
		m_graphic->set_scale_y(m_hover_scale);
	}
}

const Graphic* GraphicMenuItem::get_graphic() const {
	return m_graphic;
}

Graphic* GraphicMenuItem::get_graphic() {
	return m_graphic;
}
