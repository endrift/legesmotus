/*
 * server/ServerMap.hpp
 *
 * Copyright 2009 - Nathan Partlan, Andrew Ayer, Daniel Schneider, and Jeffrey Pfau
 * 
 */

#ifndef LM_SERVER_SERVERMAP_HPP
#define LM_SERVER_SERVERMAP_HPP

#include "common/Map.hpp"
#include "common/Point.hpp"
#include <list>

/*
 * Derives from Map, and adds spawnpoint tracking.
 */
class ServerMap : public Map {
	// Spawnpoints for each team:
	std::list<Point>	m_spawnpoints[2];
	std::list<const Point*>	m_available_spawnpoints[2];

	virtual void		add_object(PacketReader& object_data);

public:
	ServerMap();
	virtual ~ServerMap();

	virtual void	clear();

	void		reset();	// Reset the map for a new round - call before assigning spawnpoints

	// How much TOTAL space is available on the map?
	int		total_teamA_capacity() const { return m_spawnpoints[0].size(); }
	int		total_teamB_capacity() const { return m_spawnpoints[1].size(); }
	int		total_capacity() const { return total_teamA_capacity() + total_teamB_capacity(); }

	// Acquire the next available spawnpoint for the given team
	// Returns NULL if no spawnpoint is available
	const Point*	next_spawnpoint(char team);

	// Release/return the given spawnpoint to the pool
	void		return_spawnpoint(char team, const Point* p);

	bool		has_capacity(char team) const;

	// How much REAMINING space is available on the map?
	int		remaining_teamA_capacity() const { return m_available_spawnpoints[0].size(); }
	int		remaining_teamB_capacity() const { return m_available_spawnpoints[1].size(); }
	int		remaining_capacity() const { return remaining_teamA_capacity() + remaining_teamB_capacity(); }
};

#endif
