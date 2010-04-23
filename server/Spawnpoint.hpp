/*
 * server/Spawnpoint.hpp
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

#ifndef LM_SERVER_SPAWNPOINT_HPP
#define LM_SERVER_SPAWNPOINT_HPP

#include <set>
#include "common/Point.hpp"

namespace LM {
	class Spawnpoint;

	struct compare_spawnpoint {
		bool operator()(const Spawnpoint* a, const Spawnpoint* b) const;
	};

	class SpawnpointSet : private std::multiset<Spawnpoint*, compare_spawnpoint> {
		typedef std::multiset<Spawnpoint*, compare_spawnpoint> base;
	public:
		~SpawnpointSet() { clear(); }
		void			clear();
		void			reset();
		void			add(const Spawnpoint& p);
		const Spawnpoint*	acquire();
		void			release(const Spawnpoint*);

		using base::iterator;
		using base::const_iterator;
		using base::empty;
		using base::size;
	};

	class Spawnpoint {
	private:
		Point			m_point;
		Vector			m_initial_velocity;
		bool			m_is_grabbing_obstacle;

		SpawnpointSet::iterator	m_it;
		int			m_utilization;

		friend class SpawnpointSet;
		friend class compare_spawnpoint;
	public:
		Spawnpoint(Point point, Vector velocity, bool is_grabbing_obstacle);

		Point			get_point() const { return m_point; }
		Vector			get_initial_velocity() const { return m_initial_velocity; }
		bool			is_grabbing_obstacle () const { return m_is_grabbing_obstacle; }
	};

	
}

#endif
