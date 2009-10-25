/*
 * client/Hazard.cpp
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

#include "Hazard.hpp"
#include "common/Player.hpp"
#include "common/MapReader.hpp"
#include "common/team.hpp"
#include "common/math.hpp"
#include "common/timer.hpp"
#include "GameController.hpp"
#include "ClientMap.hpp"
#include <string>
#include <stdlib.h>

using namespace LM;
using namespace std;

Hazard::Hazard (Point position) : BaseMapObject(position) {
	m_graphic = NULL;
	m_params.priority = 256; // TODO: use enum
	m_is_slippery = false;
	m_team = 0;
	m_damage = 0;
	m_damage_rate = 100;
	m_is_collidable = false;
	m_collision_damage = 0;
	m_freeze_time = 0;
	m_repel_velocity = 1.0;
	m_bounce_factor = 0.9;

	m_last_damage_time = 0;
	m_angle_of_incidence = 0.0;
}

void	Hazard::collide(GameController& gc, Player& player, Point old_position, double angle_of_incidence) {
	if (!m_is_collidable) {
		return;
	}

	player.set_x(old_position.x);
	player.set_y(old_position.y);
	if ((player.is_dead() || player.is_frozen()) && !player.is_invisible()) {
		// Bounce off the wall
		player.bounce(angle_of_incidence, m_bounce_factor);
	} else if (!player.is_dead()) {
		m_angle_of_incidence = angle_of_incidence;
		if (!is_valid_team(m_team) || player.get_team() == m_team) {
			// Deal collision damage
			if (!gc.damage(m_collision_damage, NULL)) {
				// Player not killed by collision damage
				if (m_freeze_time) {
					// Freeze player based on this hazard's effects
					player.bounce(angle_of_incidence, m_bounce_factor); // Bounce off since we're frozen now
					gc.freeze(m_freeze_time);
					return; // Return now, so we don't land on the obstacle
				} else if (m_collision_damage) {
					// Remember that some damage has already been dealt
					m_last_damage_time = get_ticks();
				}
			} else {
				// Player killed
				if (repel_player(player)) {
					// Player repelled - return now, so we don't land on the obstacle
					return;
				}
			}
		}

		if (m_is_slippery) {
			player.bounce(angle_of_incidence, m_bounce_factor);
		} else {
			// Land on obstacle
			player.stop();
			player.set_is_grabbing_obstacle(true);
		}
	}
}

void	Hazard::init (MapReader& reader, ClientMap& map) {
	string		bounding_shape;

	reader >> m_graphic_name >> bounding_shape;

	while (reader.has_more()) {
		string		param_string;
		reader >> param_string;

		if (param_string == "slippery") {
			m_is_slippery = true;
		} else if (param_string == "sticky") {
			m_is_slippery = false;
		} else if (strncmp(param_string.c_str(), "team=", 5) == 0) {
			m_team = parse_team_string(param_string.c_str() + 5);
		} else if (param_string == "collidable") {
			m_is_collidable = true;
		} else if (strncmp(param_string.c_str(), "collision_damage=", 17) == 0) {
			m_collision_damage = atoi(param_string.c_str() + 17);
		} else if (strncmp(param_string.c_str(), "damage=", 7) == 0) {
			m_damage = atoi(param_string.c_str() + 7);
		} else if (strncmp(param_string.c_str(), "rate=", 5) == 0) {
			m_damage_rate = atol(param_string.c_str() + 5);
		} else if (strncmp(param_string.c_str(), "freeze=", 7) == 0) {
			m_freeze_time = atol(param_string.c_str() + 7);
		} else if (strncmp(param_string.c_str(), "repel=", 6) == 0) {
			m_repel_velocity = atof(param_string.c_str() + 6);
		} else if (strncmp(param_string.c_str(), "bounce=", 7) == 0) {
			m_bounce_factor = atof(param_string.c_str() + 7);
		} else {
			m_params.parse(param_string.c_str());
		}
	}

	m_bounding_shape.reset(ClientMap::make_bounding_shape(bounding_shape, get_position(), m_params));

	if (!m_graphic) {
		m_graphic = map.load_graphic(m_graphic_name, m_bounding_shape.get() && m_bounding_shape->is_centered(), get_position(), m_params);
	}
}

void	Hazard::interact(GameController& gc, Player& player) {
	if (player.is_dead() || player.is_frozen() || player.is_invisible() || is_valid_team(m_team) && player.get_team() != m_team) {
		m_last_damage_time = 0;
		return;
	}

	// Figure out how much damage to give the player, based on the time elapsed since the last damage was dealt
	int			damage = 0;

	if (m_last_damage_time) {
		uint64_t	now = get_ticks();
		uint64_t	time_elapsed = now - m_last_damage_time;
		damage = m_damage * (time_elapsed / m_damage_rate);
		m_last_damage_time = now - time_elapsed % m_damage_rate;
	} else {
		damage = m_damage;
		m_last_damage_time = get_ticks();
	}

	// Deal damage to the player
	if (gc.damage(damage, NULL)) {
		// Player killed
		m_last_damage_time = 0;
		if (m_is_collidable) {
			repel_player(player);
		}
	}
}

void	Hazard::disengage(GameController& gc, Player& player) {
	m_last_damage_time = 0;
}

bool	Hazard::repel_player(Player& player) {
	if (m_repel_velocity) {
		double	new_angle = get_normalized_angle(180 - m_angle_of_incidence + (((double)rand() / ((double)(RAND_MAX)+1)) - 0.5) * 180.0);
		Vector	new_velocity(Vector::make_from_magnitude(m_repel_velocity, new_angle));
		player.set_velocity(new_velocity);
		return true;
	}
	return false;
}

