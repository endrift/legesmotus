/*
 * server/ServerMap.cpp
 *
 * Copyright 2009 - Nathan Partlan, Andrew Ayer, Daniel Schneider, and Jeffrey Pfau
 * 
 */

#include "ServerMap.hpp"
#include "common/PacketReader.hpp"

ServerMap::ServerMap() {
	reset();
}

ServerMap::~ServerMap() {
	clear();
}

void	ServerMap::clear() {
	m_Aspawnpoints.clear();
	m_Bspawnpoints.clear();
	reset();
	Map::clear();
}

void	ServerMap::reset() {
	m_remaining_Aspots = m_Aspawnpoints.size();
	m_remaining_Bspots = m_Bspawnpoints.size();
	m_next_Aspawnpoint = m_Aspawnpoints.begin();
	m_next_Bspawnpoint = m_Bspawnpoints.begin();
}

// Return the next available spawnpoint for the given team
// Returns NULL if no spawn points available
const Point*	ServerMap::next_spawnpoint(char team) {
	if (team == 'A' && m_remaining_Aspots > 0) {
		--m_remaining_Aspots;
		return &*m_next_Aspawnpoint++;
	} else if (team == 'B' && m_remaining_Bspots > 0) {
		--m_remaining_Bspots;
		return &*m_next_Bspawnpoint++;
	}

	return NULL;
}

void	ServerMap::add_object(PacketReader& object_data) {
	if (object_data.packet_type() == SPAWN_POINT) {
		Point		point;
		char		team;
		object_data >> point >> team;

		if (team == 'A') {
			m_Aspawnpoints.push_back(point);
		} else if (team == 'B') {
			m_Bspawnpoints.push_back(point);
		}
	}
}

