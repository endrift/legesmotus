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
private:
	// Spawnpoints for each team:
	std::list<Point>	m_Aspawnpoints;
	std::list<Point>	m_Bspawnpoints;

	// Where the next team member should spawn:
	int					m_remaining_Aspots;
	int					m_remaining_Bspots;
	std::list<Point>::const_iterator	m_next_Aspawnpoint;
	std::list<Point>::const_iterator	m_next_Bspawnpoint;

	virtual void	add_object(PacketReader& object_data);

public:
	ServerMap();
	virtual ~ServerMap();

	virtual void	clear();

	void		reset();	// Reset the map for a new round - call before assigning spawnpoints

	// How much TOTAL space is available on the map?
	int		total_teamA_capacity() const { return m_Aspawnpoints.size(); }
	int		total_teamB_capacity() const { return m_Bspawnpoints.size(); }
	int		total_capacity() const { return total_teamA_capacity() + total_teamB_capacity(); }

	// Return the next available spawnpoint for the given team
	// Returns NULL if no spawn points available
	const Point*	next_spawnpoint(char team);

	// How much REAMINING space is available on the map?
	int		remaining_teamA_capacity() const { return m_remaining_Aspots; }
	int		remaining_teamB_capacity() const { return m_remaining_Bspots; }
	int		remaining_capacity() const { return m_remaining_Aspots + m_remaining_Bspots; }
};

#endif
