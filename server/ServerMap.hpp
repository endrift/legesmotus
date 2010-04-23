/*
 * server/ServerMap.hpp
 *
 * This file is part of Leges Motus, a networked, 2D shooter set in zero gravity.
 * 
 * Copyright 2009-2010 Andrew Ayer, Nathan Partlan, Jeffrey Pfau
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
#include "common/MapReader.hpp"
#include "Spawnpoint.hpp"
#include <list>

/*
 * Derives from Map, and adds spawnpoint tracking.
 */
namespace LM {
	class PacketWriter;

	class ServerMap : public Map {
		// Spawnpoints for each team:
		SpawnpointSet		m_spawnpoints[2];

		std::list<MapReader>	m_objects;
	
		virtual void		add_object(MapReader& object_data);

	public:
		ServerMap();
		virtual ~ServerMap();
	
		virtual void	clear();
	
		void		reset();	// Reset the map for a new round - call before assigning spawnpoints


		const std::list<MapReader>&	get_objects() const { return m_objects; }
		size_t				nbr_objects() const { return m_objects.size(); }

		// How many spawnpoints are there?
		int		nbr_spawnpoints(char team) const;
		int		nbr_spawnpoints() const { return m_spawnpoints[0].size() + m_spawnpoints[1].size(); }
	
		// How many REMAINING spawnpoints are there?
		// (At present, spawnpoints are always re-used, so this number is always equal to the number of spawnpoints)
		int		nbr_remaining_spawnpoints(char team) const { return nbr_spawnpoints(team); }
		int		nbr_remaining_spawnpoints() const { return nbr_spawnpoints(); }

		// Is there space on the map for another player?
		// (At present, spawnpoints are always re-used, so there is always space available)
		bool		has_capacity(char team) const { return true; }
		bool		has_capacity() const { return has_capacity('A') || has_capacity('B'); }
	
		// Acquire the next available spawnpoint for the given team
		// Returns NULL if no spawnpoint is available
		const Spawnpoint* next_spawnpoint(char team);
	
		// Release/return the given spawnpoint to the pool
		void		return_spawnpoint(char team, const Spawnpoint* p);
	
	};

	inline PacketWriter& operator<<(PacketWriter& packet, const ServerMap& server_map) {
		const Map& map(server_map);
		return packet << map;
	}
}

#endif
