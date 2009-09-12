/*
 * server/ServerMap.cpp
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

#include "ServerMap.hpp"
#include "common/MapReader.hpp"
#include "common/team.hpp"

using namespace LM;
using namespace std;

ServerMap::ServerMap() {
	reset();
}

ServerMap::~ServerMap() {
	clear();
}

void	ServerMap::clear() {
	m_spawnpoints[0].clear();
	m_spawnpoints[1].clear();
	m_objects.clear();
	Map::clear();
}

void	ServerMap::reset() {
	m_spawnpoints[0].reset();
	m_spawnpoints[1].reset();
}

int	ServerMap::nbr_spawnpoints(char team) const {
	if (is_valid_team(team)) {
		return m_spawnpoints[team - 'A'].size();
	}
	return 0;
}

// Acquire the next available spawnpoint for the given team
const Spawnpoint* ServerMap::next_spawnpoint(char team) {
	if (is_valid_team(team)) {
		return m_spawnpoints[team - 'A'].acquire();
	}
	return NULL;
}

// Release/return the given spawnpoint to the pool
void	ServerMap::return_spawnpoint(char team, const Spawnpoint* spawnpoint) {
	if (is_valid_team(team)) {
		m_spawnpoints[team - 'A'].release(spawnpoint);
	}
}

void	ServerMap::add_object(MapReader& object_data) {
	// Save the object data in the m_objects list
	m_objects.push_back(object_data);
	
	if (object_data.get_type() == SPAWN_POINT) {
		Point		point;
		string		team_string;
		object_data >> point >> team_string;

		Vector		initial_velocity;
		bool		is_grabbing_obstacle = true;
		while (object_data.has_more()) {
			string		param_string;
			object_data >> param_string;

			if (param_string == "notgrabbing")
				is_grabbing_obstacle = false;
			else if (strncmp(param_string.c_str(), "velocity=", 9) == 0)
				initial_velocity = Vector::make_from_string(param_string.c_str() + 9);
		}

		char		team = parse_team_string(team_string.c_str());
		if (is_valid_team(team)) {
			m_spawnpoints[team - 'A'].add(Spawnpoint(point, initial_velocity, is_grabbing_obstacle));
		}
	}
}

