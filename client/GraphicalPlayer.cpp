/*
 * client/GraphicalPlayer.cpp
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

#include "GraphicalPlayer.hpp"
#include "Sprite.hpp"
#include "common/math.hpp"

#include <iostream>

using namespace std;

GraphicalPlayer::GraphicalPlayer() : Player::Player() {
	m_sprite = NULL;
}

GraphicalPlayer::GraphicalPlayer(const char* name, uint32_t id, char team, GraphicGroup* sprite, double x, double y, double xvel, double yvel, double rotation)
: Player::Player(name, id, team, x, y, xvel, yvel, rotation) {
	m_sprite = sprite;
	if (m_sprite != NULL) {
		m_sprite->set_x(x);
		m_sprite->set_y(y);
		m_sprite->set_rotation(rotation);
	}
}

GraphicalPlayer::~GraphicalPlayer() {
	
}

/*double GraphicalPlayer::get_width() {
	if (m_sprite != NULL) {
		return m_sprite->get_width();
	} else {
		return 0;
	}
}

double GraphicalPlayer::get_height() {
	if (m_sprite != NULL) {
		return m_sprite->get_height();
	} else {
		return 0;
	}
}*/

void GraphicalPlayer::set_sprite(GraphicGroup* s) {
	m_sprite = s;
	if (m_sprite != NULL) {
		m_sprite->set_x(m_x);
		m_sprite->set_y(m_y);
		m_sprite->set_rotation(m_rotation);
	}
}

void GraphicalPlayer::set_name_sprite(Graphic* ns) {
	m_name_sprite = ns;
}

void GraphicalPlayer::set_radius(double radius) {
	if (radius >= 0) {
		m_bounding_radius = radius;
	} else {
		cerr << "Error: Radius of player cannot be less than 0. Was: " << radius << endl;
	}
}

void GraphicalPlayer::set_is_invisible(bool is_invisible) {
	m_is_invisible = is_invisible;
	if (m_sprite != NULL) {
		if (is_invisible) {
			m_sprite->set_invisible(true);
		} else {
			m_sprite->set_invisible(false);
		}
	}
}

void GraphicalPlayer::set_x(double x) {
	m_x = x;
	if (m_sprite != NULL) {
		m_sprite->set_x(x);
	}
}

void GraphicalPlayer::set_y(double y) {
	m_y = y;
	if (m_sprite != NULL) {
		m_sprite->set_y(y);
	}
}

void GraphicalPlayer::set_rotation_degrees(double rotation) {
	m_rotation = get_normalized_angle(rotation);
	if (m_sprite != NULL) {
		m_sprite->set_rotation(m_rotation);
	}
}

void GraphicalPlayer::set_rotation_radians(double rotation) {
	m_rotation = get_normalized_angle(rotation * RADIANS_TO_DEGREES);
	if (m_sprite != NULL) {
		m_sprite->set_rotation(m_rotation);
	}
}

void GraphicalPlayer::set_is_frozen(bool is_frozen) {
	m_is_frozen = is_frozen;
	
	if (m_is_frozen == true) {
		m_sprite->set_red_intensity(0.7);
		m_sprite->set_green_intensity(0.7);
		m_sprite->set_blue_intensity(0.7);
	} else {
		m_sprite->set_red_intensity(1);
		m_sprite->set_green_intensity(1);
		m_sprite->set_blue_intensity(1);
	}
}
