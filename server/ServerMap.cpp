/*
 * server/ServerMap.cpp
 *
 * Copyright 2009 - Nathan Partlan, Andrew Ayer, Daniel Schneider, and Jeffrey Pfau
 * 
 */

#include "ServerMap.hpp"
#include "common/PacketReader.hpp"
#include "common/team.hpp"
#include <algorithm>
#include <iterator>

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

void	ServerMap::add_object(PacketReader& object_data) {
	if (object_data.packet_type() == SPAWN_POINT) {
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
