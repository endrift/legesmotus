/*
 * client/ForceField.cpp
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

#include "ForceField.hpp"
#include "ClientMap.hpp"
#include "GameController.hpp"
#include "common/MapReader.hpp"
#include "common/team.hpp"
#include <string>
#include <cstring>
#include <cstdlib>

using namespace LM;
using namespace std;

ForceField::ForceField(Point position) : BaseMapObject(position) {
	m_graphic = NULL;
	m_team = 0;
	m_affects_frozen = false;
	m_affects_thawed = false;
	m_sucks_off_obstacles = false;
	m_friction = 0.0;
	m_gravity = 0.0;
	m_params.priority = 512; // TODO: use enum
}

void	ForceField::interact (GameController& gc, Player& player) {
	if (player.is_invisible()) {
		return;
	}

	if (is_valid_team(m_team) && m_team != player.get_team()) {
		return;
	}

	bool	is_frozen = player.is_frozen() || player.is_dead();

	if ((is_frozen && m_affects_frozen || !is_frozen && m_affects_thawed) && (m_sucks_off_obstacles || !player.is_grabbing_obstacle())) {
		// Apply vector force
		player.set_velocity(player.get_velocity() + m_force);

		if (m_friction != 0.0) {
			// Apply scalar friction
			player.set_velocity(player.get_velocity() - Vector::make_from_magnitude(m_friction, player.get_velocity().get_angle()));
		}

		if (m_gravity != 0.0) {
			// Apply scalar gravity
			double distance = Point::distance(get_position(), player.get_position());
			if (distance != 0.0) {
				double accel = m_gravity / (distance * distance);
				double angle = (get_position() - player.get_position()).get_angle();
				player.set_velocity(player.get_velocity() + Vector::make_from_magnitude(accel, angle));
			}
		}
	}
}

void	ForceField::init (MapReader& reader, ClientMap& map) {
	string		bounding_shape;

	reader >> m_graphic_name >> bounding_shape;

	while (reader.has_more()) {
		string		param_string;
		reader >> param_string;
		if (strncmp(param_string.c_str(), "team=", 5) == 0) {
			m_team = parse_team_string(param_string.c_str() + 5);
		} else if (strncmp(param_string.c_str(), "force=", 6) == 0) {
			m_force = Vector::make_from_string(param_string.c_str() + 6);
		} else if (strncmp(param_string.c_str(), "friction=", 9) == 0) {
			m_friction = atof(param_string.c_str() + 9);
		} else if (strncmp(param_string.c_str(), "gravity=", 8) == 0) {
			m_gravity = atof(param_string.c_str() + 8);
		} else if (param_string == "nofrozen") {
			m_affects_frozen = false;
		} else if (param_string == "frozen") {
			m_affects_frozen = true;
		} else if (param_string == "nothawed") {
			m_affects_thawed = false;
		} else if (param_string == "thawed") {
			m_affects_thawed = true;
		} else if (param_string == "sucks-off-obstacles") {
			m_sucks_off_obstacles = true;
		} else {
			m_params.parse(param_string.c_str());
		} 
	}

	m_bounding_shape.reset(ClientMap::make_bounding_shape(bounding_shape, get_position(), m_params));

	if (!m_graphic) {
		m_graphic = map.load_graphic(m_graphic_name, m_bounding_shape.get() && m_bounding_shape->is_centered(), get_position(), m_params);
	}
}

