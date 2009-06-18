/*
 * server/ServerMap.hpp
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

	virtual void		add_object(MapReader& object_data);

public:
	ServerMap();
	virtual ~ServerMap();

	virtual void	clear();

	void		reset();	// Reset the map for a new round - call before assigning spawnpoints

	// How much TOTAL space is available on the map?
	int		total_teamA_capacity() const { return m_spawnpoints[0].size(); }
	int		total_teamB_capacity() const { return m_spawnpoints[1].size(); }
	int		total_capacity(char team) const;
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
