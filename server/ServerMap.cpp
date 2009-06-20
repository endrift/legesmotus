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
#include <algorithm>
#include <iterator>

using namespace LM;
using namespace std;

struct address_of {
	template<class T> T*	operator() (T& r) const { return &r; }
};

ServerMap::ServerMap() {
	reset();
}

ServerMap::~ServerMap() {
	clear();
}

void	ServerMap::clear() {
	m_spawnpoints[0].clear();
	m_spawnpoints[1].clear();
	m_available_spawnpoints[0].clear();
	m_available_spawnpoints[1].clear();
	Map::clear();
}

void	ServerMap::reset() {
	m_available_spawnpoints[0].clear();
	m_available_spawnpoints[1].clear();
	std::transform(m_spawnpoints[0].begin(), m_spawnpoints[0].end(), back_inserter(m_available_spawnpoints[0]), address_of());
	std::transform(m_spawnpoints[1].begin(), m_spawnpoints[1].end(), back_inserter(m_available_spawnpoints[1]), address_of());
}

// Acquire the next available spawnpoint for the given team
const Point* ServerMap::next_spawnpoint(char team) {
	if (is_valid_team(team) && has_capacity(team)) {
		const Point* p(m_available_spawnpoints[team - 'A'].front());
		m_available_spawnpoints[team - 'A'].pop_front();
		return p;
	}
	return NULL;
}

void	ServerMap::return_spawnpoint(char team, const Point* spawnpoint) {
	if (is_valid_team(team)) {
		m_available_spawnpoints[team - 'A'].push_back(spawnpoint);
	}
}

bool	ServerMap::has_capacity(char team) const {
	return is_valid_team(team) && !m_available_spawnpoints[team - 'A'].empty();
}

void	ServerMap::add_object(MapReader& object_data) {
	if (object_data.get_type() == SPAWN_POINT) {
		Point		point;
		char		team;
		object_data >> point >> team;

		if (is_valid_team(team)) {
			m_spawnpoints[team - 'A'].push_back(point);
		}
	}
}

int	ServerMap::total_capacity(char team) const {
	return is_valid_team(team) ? m_spawnpoints[team - 'A'].size() : 0;
}
