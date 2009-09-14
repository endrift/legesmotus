/*
 * server/Spawnpoint.cpp
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

#include "Spawnpoint.hpp"

using namespace LM;
using namespace std;


bool compare_spawnpoint::operator()(const Spawnpoint* a, const Spawnpoint* b) const {
	return a->m_utilization < b->m_utilization;
}

void	SpawnpointSet::clear() {
	for (const_iterator it(begin()); it != end(); ++it)
		delete *it;
	base::clear();
}

void	SpawnpointSet::reset() {
	// Create a new set, and move all the spawnpoints over to it, resetting the utilization to 0 as we go.
	base			new_set;
	const_iterator		it(begin());
	while (it != end()) {
		Spawnpoint*	p = *it;
		erase(it++);
		p->m_utilization = 0;
		p->m_it = new_set.insert(p);
	}
	// Then swap in the new set.
	swap(new_set);
}

void	SpawnpointSet::add(const Spawnpoint& pt) {
	Spawnpoint* sp = new Spawnpoint(pt);
	sp->m_it = insert(sp);
}

const Spawnpoint*	SpawnpointSet::acquire() {
	if (empty()) {
		return NULL;
	}

	Spawnpoint* p = *begin();
	erase(begin());
	++p->m_utilization;
	p->m_it = insert(p);
	return p;
}

void	SpawnpointSet::release(const Spawnpoint* cp) {
	Spawnpoint* p = *cp->m_it;
	erase(p->m_it);
	--p->m_utilization;
	p->m_it = insert(p);
}

Spawnpoint::Spawnpoint(Point point, Vector velocity, bool is_grabbing_obstacle) : m_point(point), m_initial_velocity(velocity), m_is_grabbing_obstacle(is_grabbing_obstacle) {
	m_utilization = 0;
}
