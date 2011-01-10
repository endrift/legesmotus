/*
 * gui/Hud.cpp
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

#include "Hud.hpp"

using namespace LM;
using namespace std;

const Color Hud::BLUE_BRIGHT(0xFA, 0xFA, 0xFF);
const Color Hud::BLUE_SHADOW(0x5E, 0x55, 0x42);
const Color Hud::BLUE_DARK(0x79, 0x8B, 0xB5);

const Color Hud::RED_BRIGHT(0xFF, 0xFA, 0xFA);
const Color Hud::RED_SHADOW(0x42, 0x55, 0x5E);
const Color Hud::RED_DARK(0xB5, 0x8B, 0x79);

const int Hud::m_shadow_convolve_height = 5;
const int Hud::m_shadow_convolve_width = 5;
const float Hud::m_shadow_convolve_data[] = {
	0.0f, 1.0f, 1.0f, 1.0f, 0.0f,
	1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
	0.0f, 1.0f, 1.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
};

const Color& Hud::get_team_color(char team, ColorType type) {
	switch (team) {
	case 'A':
		switch (type) {
		case COLOR_BRIGHT:
			return BLUE_BRIGHT;
		case COLOR_SHADOW:
			return BLUE_SHADOW;
		case COLOR_DARK:
			return BLUE_DARK;
		}
		break;
	case 'B':
		switch (type) {
		case COLOR_BRIGHT:
			return RED_BRIGHT;
		case COLOR_SHADOW:
			return RED_SHADOW;
		case COLOR_DARK:
			return RED_DARK;
		}
		break;
	}
	return Color::BLACK;
}

Hud::Hud(Widget* parent) : Widget(parent), m_shadow_kernel(m_shadow_convolve_data, m_shadow_convolve_width, m_shadow_convolve_height, 1) {
	// Nothing to do
}

const ConvolveKernel* Hud::get_shadow_kernel() const {
	return &m_shadow_kernel;
}
