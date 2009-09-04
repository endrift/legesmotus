/*
 * client/Obstacle.cpp
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

#include "Obstacle.hpp"
#include "common/Player.hpp"
#include "common/MapReader.hpp"
#include "GameController.hpp"
#include "ClientMap.hpp"
#include <string>

using namespace LM;
using namespace std;

Obstacle::Obstacle (Point position) : BaseMapObject(position) {
	m_graphic = NULL;
	m_params.priority = 256; // TODO: use enum
	m_is_slippery = false;
}

void	Obstacle::collide(GameController& gc, Player& player, Point old_position, double angle_of_incidence) {
	player.set_x(old_position.x);
	player.set_y(old_position.y);
	if (m_is_slippery || player.is_frozen() && !player.is_invisible()) {
		// Bounce off the wall
		player.bounce(angle_of_incidence, 0.9);
	} else {
		// Stop moving
		player.stop();

		// Rotate to a good orientation:
		//gc.rotate_towards_angle(angle_of_incidence, GameController::ROTATION_ADJUST_SPEED);
	}
}

void	Obstacle::init (MapReader& reader, ClientMap& map) {
	string		bounding_shape;

	reader >> m_graphic_name >> bounding_shape;

	while (reader.has_more()) {
		string		param_string;
		reader >> param_string;

		if (param_string == "slippery") {
			m_is_slippery = true;
		} else if (param_string == "sticky") {
			m_is_slippery = false;
		} else {
			m_params.parse(param_string.c_str());
		}
	}

	m_bounding_shape.reset(ClientMap::make_bounding_shape(bounding_shape, get_position(), m_params));

	if (!m_graphic) {
		m_graphic = map.load_graphic(m_graphic_name, m_bounding_shape.get() && m_bounding_shape->is_centered(), get_position(), m_params);
	}
}

