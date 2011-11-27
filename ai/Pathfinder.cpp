/*
 * ai/Pathfinder.cpp
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

#include "Pathfinder.hpp"
#include "SparseIntersectMap.hpp"
#include "common/Player.hpp"
#include "common/physics.hpp"
#include <queue>
#include <set>
#include <map>
#include "common/misc.hpp"

using namespace LM;
using namespace std;

std::map<SparseIntersectMap::Intersect, float, Pathfinder::IntersectComparator> Pathfinder::f_scores;
std::map<SparseIntersectMap::Intersect, float, Pathfinder::IntersectComparator> Pathfinder::g_scores;
std::map<SparseIntersectMap::Intersect, float, Pathfinder::IntersectComparator> Pathfinder::h_scores;

Pathfinder::Pathfinder() {
	set_graph(NULL);
	m_timeout = -1;
}

Pathfinder::Pathfinder(SparseIntersectMap* graph) {
	set_graph(graph);
	m_timeout = -1;
}

Pathfinder::~Pathfinder() {
	clear_avoid_areas();
}

void Pathfinder::set_graph(SparseIntersectMap* graph) {
	m_graph = graph;
}

void Pathfinder::set_physics(const b2World* world) {
	m_ray_cast.set_physics(world);
}

void Pathfinder::set_timeout(long timeout) {
	m_timeout = timeout;
}

bool Pathfinder::find_path(float start_x, float start_y, float goal_x, float goal_y, float tolerance, vector<SparseIntersectMap::Intersect>& path, PathFoundFunc check_found) {
	f_scores.clear();
	g_scores.clear();
	h_scores.clear();
	
	uint64_t start_time = get_ticks();
	
	// Check if we're already there.
	b2Vec2 curr_dist = b2Vec2(goal_x - start_x, goal_y - start_y);
	if (curr_dist.Length() <= tolerance) {
		return true;
	}
	
	set<SparseIntersectMap::Intersect, IntersectComparator> closed_set; // Stores nodes already visited.
	map<SparseIntersectMap::Intersect, SparseIntersectMap::Intersect, IntersectComparator> came_from; // Stores where we came from to get to each node.
	priority_queue<SparseIntersectMap::Intersect, vector<SparseIntersectMap::Intersect>, GraphNodeComparator> open_set; // The nodes we have yet to check.
	
	// Set up the initial node to check.
	SparseIntersectMap::Intersect start;
	start.x = start_x;
	start.y = start_y;
	start.dist = 0;
	g_scores[start] = 0;
	h_scores[start] = estimate_h_score(start, goal_x, goal_y);
	f_scores[start] = h_scores[start];
	
	open_set.push(start);
	
	float theta_change = m_graph->get_granularity_theta();
	
	while (!open_set.empty()) {
		SparseIntersectMap::Intersect current = open_set.top();
		//DEBUG("Current: " << current.x << ", " << current.y);
		if ((*this.*check_found)(current, goal_x, goal_y, tolerance)) {
			// We're done.
			reconstruct_path(came_from, current, path);
			//DEBUG("Found a path! Distance: " << g_scores[current] << " Hops: " << path.size());
			return true;
		}
		open_set.pop();
		
		closed_set.insert(current);
		
		// XXX: We need a better way to iterate through the neighbors, without having to check _every_ angle
		for (float theta = 0; theta < 360; theta += theta_change) {
			SparseIntersectMap::Intersect neighbor;
			
			// If this neighbor doesn't exist, or we've already checked it, skip it:
			if (!m_graph->get(current.x, current.y, theta, &neighbor)) {
				continue;
			}
			
			if (closed_set.find(neighbor) != closed_set.end()) {
				continue;
			}
			
			float tentative_g_score = g_scores[current] + calculate_g_score(neighbor);
			
			if (g_scores.find(neighbor) == g_scores.end() || g_scores[neighbor] > tentative_g_score) {
				g_scores[neighbor] = tentative_g_score;
				h_scores[neighbor] = estimate_h_score(neighbor, goal_x, goal_y);
				f_scores[neighbor] = g_scores[neighbor] + h_scores[neighbor];
				
				// XXX: We need a better way to update things that are already in the open_set, other than pushing them again.
				open_set.push(neighbor);
				came_from[neighbor] = current;
			}
		
			if (m_timeout != -1 && get_ticks() > start_time + m_timeout) {
				return false;
			}
		}
	}
	
	return false;
}

bool Pathfinder::find_path(float start_x, float start_y, float goal_x, float goal_y, float tolerance, vector<SparseIntersectMap::Intersect>& path) {
	return find_path(start_x, start_y, goal_x, goal_y, tolerance, path, &Pathfinder::is_within_dist);
}

bool Pathfinder::find_path_to_visibility(float start_x, float start_y, float goal_x, float goal_y, float tolerance, vector<SparseIntersectMap::Intersect>& path) {
	return find_path(start_x, start_y, goal_x, goal_y, tolerance, path, &Pathfinder::is_visible);
}

void Pathfinder::reconstruct_path(std::map<SparseIntersectMap::Intersect, SparseIntersectMap::Intersect, IntersectComparator>& came_from, SparseIntersectMap::Intersect current, std::vector<SparseIntersectMap::Intersect>& path) {
	if (came_from.find(current) != came_from.end()) {
		reconstruct_path(came_from, came_from[current], path);
	}
	path.push_back(current);
}

float Pathfinder::get_dist(SparseIntersectMap::Intersect node, float goal_x, float goal_y) {
	return sqrt((goal_x - node.x) * (goal_x - node.x) + (goal_y - node.y) * (goal_y - node.y));
}

bool Pathfinder::is_within_dist(SparseIntersectMap::Intersect node, float goal_x, float goal_y, float tolerance) {
	return get_dist(node, goal_x, goal_y) < tolerance;
}

bool Pathfinder::is_visible(SparseIntersectMap::Intersect node, float goal_x, float goal_y, float tolerance) {
	if (!is_within_dist(node, goal_x, goal_y, tolerance)) {
		return false;
	}
	
	float direction = atan2(goal_y - node.y, goal_x - node.x);
	m_ray_cast.do_ray_cast(b2Vec2(to_physics(node.x), to_physics(node.y)), direction);
	
	return to_game(m_ray_cast.get_result().shortest_dist) >= get_dist(node, goal_x, goal_y) - 40;
}

float Pathfinder::estimate_h_score(SparseIntersectMap::Intersect node, float goal_x, float goal_y) {
	// TODO: Is there a better heuristic?
	return get_dist(node, goal_x, goal_y);
}

float Pathfinder::calculate_g_score(SparseIntersectMap::Intersect node) {
	float g_score = node.dist;
	for (unsigned int i = 0; i < m_avoid_areas.size(); i++) {
		AvoidArea* undesired_loc = m_avoid_areas[i];
		g_score += max(0.0f, (undesired_loc->area - get_dist(node, undesired_loc->x, undesired_loc->y))/undesired_loc->area * undesired_loc->weight);
	}
	return g_score;
}

void Pathfinder::add_avoid_area(AvoidArea* a) {
	m_avoid_areas.push_back(a);
}
		
void Pathfinder::clear_avoid_areas() {
	m_avoid_areas.clear();
}
