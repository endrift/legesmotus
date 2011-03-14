/*
 * ai/Pathfinder.hpp
 *
 * This file is part of Leges Motus, a networked, 2D shooter set in zero gravity.
 * 
 * Copyright 2009-2011 Andrew Ayer, Nathan Partlan, Jeffrey Pfau
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

#ifndef LM_AI_PATHFINDER_HPP
#define LM_AI_PATHFINDER_HPP

#include "SparseIntersectMap.hpp"
#include <queue>
#include <vector>
#include <map>

namespace LM {
	class Player;

	class Pathfinder {
	private:
	
		class IntersectComparator {
		public:
			bool operator()(const SparseIntersectMap::Intersect& n1, const SparseIntersectMap::Intersect& n2) {
				return (n1.dist > n2.dist);
			}
		};
		
	public:
	
		static std::map<SparseIntersectMap::Intersect, float, IntersectComparator> f_scores;
		static std::map<SparseIntersectMap::Intersect, float, IntersectComparator> g_scores;
		static std::map<SparseIntersectMap::Intersect, float, IntersectComparator> h_scores;
	
	private:
	
		class GraphNodeComparator {
		public:
			bool operator()(SparseIntersectMap::Intersect& n1, SparseIntersectMap::Intersect& n2) {
				return (Pathfinder::f_scores[n1] > Pathfinder::f_scores[n2]);
			}
		};
		
		SparseIntersectMap* m_graph;
	
		float estimate_h_score(SparseIntersectMap::Intersect intersect, float goal_x, float goal_y);
	
		float get_dist(SparseIntersectMap::Intersect node, float goal_x, float goal_y);
		
		void reconstruct_path(std::map<SparseIntersectMap::Intersect, SparseIntersectMap::Intersect, IntersectComparator>& came_from, SparseIntersectMap::Intersect current, std::vector<SparseIntersectMap::Intersect>& path);
	
	public:
		Pathfinder();
		Pathfinder(SparseIntersectMap* graph);
	
		void set_graph(SparseIntersectMap* graph);
		
		// Fills the "path" with the path to the goal. Returns false if it cannot find one.
		bool find_path(float start_x, float start_y, float goal_x, float goal_y, float tolerance, std::vector<SparseIntersectMap::Intersect>& path);
	};
}

#endif
