/*
 * ai/MapGrapher.cpp
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

#include "MapGrapher.hpp"
#include "SparseIntersectMap.hpp"
#include "common/Map.hpp"
#include "common/physics.hpp"
#include "common/RayCast.hpp"
#include "common/MapObject.hpp"
#include "common/GameLogic.hpp"
#include "common/Player.hpp"
#include <fstream>

using namespace LM;
using namespace std;

const int MapGrapher::GRANULARITY = 5;
const int MapGrapher::MAX_SIZE = 1000000;
const int MapGrapher::BUFFER_ANGLE = 30;
const float MapGrapher::MULTI_CAST_WIDTH = .9f;
const bool MapGrapher::MULTI_CAST = true;

MapGrapher::MapGrapher() {
	m_physics = NULL;
	m_graph = NULL;
	
	objects_mapped = 0;
	sides_mapped = 0;
	entries_mapped = 0;
	entries_skipped = 0;
}

MapGrapher::~MapGrapher() {
	delete m_graph;
}

void MapGrapher::map_object(b2Body* body) {
	PhysicsObject* obj = static_cast<PhysicsObject*>(body->GetUserData());
	if (obj->get_type() == PhysicsObject::MAP_OBJECT) {
		MapObject* object = static_cast<MapObject*>(obj);
		if (!object->is_jumpable()) {
			return;
		}
	}
	
	b2Fixture* fixture = body->GetFixtureList();
	
	b2Vec2 body_pos = body->GetPosition();
	
	// For each fixture:
	while (fixture != NULL) {
		if (fixture->IsSensor()) {
			fixture = fixture->GetNext();
			continue;
		}
	
		b2Shape* shape = fixture->GetShape();
		if (shape->GetType() != b2Shape::e_polygon) {
			// TODO: Handle non-polygon bounding shapes?
			fixture = fixture->GetNext();
			continue;
		}
		
		b2PolygonShape* polyshape = static_cast<b2PolygonShape*>(shape);
		int index = 0;
		
		// Compute the world point, and put it in the set of lines.
		b2Vec2 last_vertex;
		while (index < polyshape->GetVertexCount()) {
			b2Vec2 world_vertex = body->GetWorldPoint(polyshape->GetVertex(index));
			
			if (index > 0) {
				map_segment(last_vertex, world_vertex, obj);
			}
			
			last_vertex.x = world_vertex.x;
			last_vertex.y = world_vertex.y;
			index++;
		}
		// Map the last vertex back to the first.
		map_segment(last_vertex, body->GetWorldPoint(polyshape->GetVertex(0)), obj);
		
		fixture = fixture->GetNext();
	}
	
	objects_mapped++;
}

void MapGrapher::map_segment(b2Vec2 start, b2Vec2 end, PhysicsObject* obj) {
	start.x = to_game(start.x);
	start.y = to_game(start.y);
	end.x = to_game(end.x);
	end.y = to_game(end.y);
	b2Vec2 temp_vec(start.x, start.y);
	temp_vec -= end;
	float length = temp_vec.Length();
	temp_vec += end;
	float dir = get_normalized_angle(to_degrees(atan2(end.y - start.y, end.x - start.x)));
	
	SparseIntersectMap::Intersect isect;
	
	int entries_checked = 0;
	
	// Ignore sides that are outside the map and oriented outwards.
	float dirnorm = get_normalized_angle(dir-90);
	if (start.x < 0 && dirnorm > 90 && dirnorm < 270) {
		return;
	}

	if (start.x > m_width && (dirnorm < 90 || dirnorm > 270)) {
		return;
	}
	
	if (start.y < 0 && dirnorm > 180 && dirnorm <= 359) {
		return;
	}
	
	if (start.y > m_height && dirnorm < 180 && dirnorm >= 1) {
		return;
	}
	
	// Go a little ways into the shape, so we don't accidentally start inside another shape as well,
	// which would cause Box2D to ignore the other shape when raycasting, and give us incorrect results.
	temp_vec.x = temp_vec.x - 1.0 * cos(to_radians(dir - 90));
	temp_vec.y = temp_vec.y - 1.0 * sin(to_radians(dir - 90));
	
	b2Vec2 temp_vec2(temp_vec.x, temp_vec.y);
	b2Vec2 temp_vec3(temp_vec.x, temp_vec.y);
	
	for (int dist = 0; dist <= length; dist += dist_change) {
		for (float angle = dir - BUFFER_ANGLE - theta_change; angle > dir - (180-BUFFER_ANGLE) + theta_change; angle -= theta_change) {
			
			float normalized_angle = to_radians(get_normalized_angle(angle));
			
			// Check if we already have a match for this angle:
			if (m_graph->get(temp_vec.x, temp_vec.y, to_degrees(normalized_angle), &isect)) {
				entries_skipped++;
				continue;
			}
			
			entries_checked++;
			
			// Cast a ray where the player's head would be.
			m_ray_cast.do_ray_cast(b2Vec2(to_physics(temp_vec.x), to_physics(temp_vec.y)), normalized_angle, -1, obj);
			
			RayCast::RayCastResult& result = m_ray_cast.get_result();
			
			if (MULTI_CAST) {
				// Cast a second ray where the player's feet would be.
				temp_vec2.x = temp_vec.x + to_physics(Player::PLAYER_HEIGHT*MULTI_CAST_WIDTH) * cos(to_radians(dir));
				temp_vec2.y = temp_vec.y + to_physics(Player::PLAYER_HEIGHT*MULTI_CAST_WIDTH) * sin(to_radians(dir));
			
				m_ray_cast2.do_ray_cast(b2Vec2(to_physics(temp_vec2.x), to_physics(temp_vec2.y)), normalized_angle, -1, obj);
			
				RayCast::RayCastResult& result2 = m_ray_cast2.get_result();
				
				// Cast a third ray where the player's feet would be on the other side.
				temp_vec3.x = temp_vec.x + to_physics(Player::PLAYER_HEIGHT*MULTI_CAST_WIDTH) * cos(to_radians(dir-180));
				temp_vec3.y = temp_vec.y + to_physics(Player::PLAYER_HEIGHT*MULTI_CAST_WIDTH) * sin(to_radians(dir-180));
			
				m_ray_cast3.do_ray_cast(b2Vec2(to_physics(temp_vec3.x), to_physics(temp_vec3.y)), normalized_angle, -1, obj);
			
				RayCast::RayCastResult& result3 = m_ray_cast3.get_result();
			
				if (result.shortest_dist == -1 && result2.shortest_dist == -1 && result3.shortest_dist == -1) {
					continue;
				}
			
				if (result.shortest_dist == -1 || (result.shortest_dist > result2.shortest_dist && result2.shortest_dist != -1)) {
					result = result2;
				}
				
				if (result.shortest_dist == -1 || (result.shortest_dist > result3.shortest_dist && result3.shortest_dist != -1)) {
					result = result3;
				}
			} else {
				if (result.shortest_dist == -1) {
					continue;
				}
			}
			
			entries_mapped++;
			
			isect.x = to_game(result.hit_point.x);
			isect.y = to_game(result.hit_point.y);
			isect.dist = to_game(result.shortest_dist);
			
			if (isect.dist <= 50) {
				entries_skipped++;
				// Don't store the result if you will travel a very short distance.
				continue;
			}
			
			//DEBUG("Started at: " << temp_vec.x << ", " << temp_vec.y << ", " << to_degrees(normalized_angle) << ", HIT: " << to_game(result.hit_point.x) << ", " << to_game(result.hit_point.y));
			
			// Set the forward direction in the graph.
			m_graph->set(temp_vec.x, temp_vec.y, to_degrees(normalized_angle), isect);
			
			// Set the reverse direction in the graph.
			if (m_graph->get(to_game(result.hit_point.x), to_game(result.hit_point.y), get_normalized_angle(angle-180), &isect)) {
				entries_skipped++;
				//DEBUG("Reverse entry at " << to_game(result.hit_point.x) << ", " << to_game(result.hit_point.y) << ", " << get_normalized_angle(angle-180) << " already set.");
				continue;
			}
			
			isect.x = temp_vec.x;
			isect.y = temp_vec.y;
			isect.dist = to_game(result.shortest_dist);
			
			entries_mapped++;
			
			//DEBUG("Setting: " << to_game(result.hit_point.x) << ", " << to_game(result.hit_point.y) << ", " << get_normalized_angle(angle-180));
			
			m_graph->set(to_game(result.hit_point.x), to_game(result.hit_point.y), get_normalized_angle(angle-180), isect);
		}
		temp_vec.x += dist_change * cos(to_radians(dir));
		temp_vec.y += dist_change * sin(to_radians(dir));
	}
	
	//DEBUG("Entries checked: " << entries_checked);
	
	sides_mapped++;
}

void MapGrapher::load_map(const GameLogic* logic, b2World* world) {
	start_time = get_ticks();
	
	const Map* map = logic->get_map();
	if (map == NULL) {
		WARN("Error: Tried to graph a null map.");
		return;
	}
	
	m_width = map->get_width();
	m_height = map->get_height();
	
	m_physics = world;
	
	m_ray_cast.set_physics(world);
	m_ray_cast2.set_physics(world);
	
	// Clear our current map:
	delete m_graph;

	ifstream graph_file;
	open_resource(&graph_file, (string("maps") + PATH_SEP + map->get_name() + ".mgf").c_str(), true);
	bool res_used = false;

	if (graph_file.fail()) {
		m_graph = new SparseIntersectMap(GRANULARITY, MAX_SIZE);
	} else {
		res_used = true;
		m_graph = new SparseIntersectMap(&graph_file);
	}
	graph_file.close();
	
	theta_change = m_graph->get_granularity_theta();
	
	// Note: We test more distances than necessary, in order to get the edges of bins we might otherwise have missed.
	dist_change = sqrt(m_graph->get_granularity_x() * m_graph->get_granularity_x() + m_graph->get_granularity_y() * m_graph->get_granularity_y())*0.7f;
	
	// Initialize our list of objects to be done.
	m_objects.clear();

	if (!res_used) {
		b2Body* body = m_physics->GetBodyList();
		while (body != NULL) {
			PhysicsObject* obj = static_cast<PhysicsObject*>(body->GetUserData());
			if (obj->get_type() == PhysicsObject::MAP_EDGE || obj->get_type() == PhysicsObject::MAP_OBJECT) {
				m_objects.push_back(body);
			}
			body = body->GetNext();
		}
	}
}

void MapGrapher::do_mapping(int num_objects) {
	if (m_physics == NULL) {
		return;
	}
	
	if (num_objects == -1) {
		num_objects = m_objects.size();
	}
	
	for (int i = 0; i < num_objects; i++) {
		if (m_objects.empty()) {
			return;
		}
	
		b2Body* next_obj = m_objects.back();
	
		// Map the next object.
		map_object(next_obj);
	
		// We are now done with the next object.
		m_objects.pop_back();
	}
	
	if (is_done_mapping()) {
		DEBUG("Objects: " << objects_mapped << " Sides: " << sides_mapped << " Entries: " << entries_mapped << " Skipped: " << entries_skipped << " (" << (((float)entries_skipped/(entries_skipped+entries_mapped)) * 100) << "%)");
		DEBUG("Mapping completed in " << ((get_ticks() - start_time)/1000.0f) << " seconds.");
	}
}

bool MapGrapher::is_done_mapping() const {
	return m_objects.empty();
}

bool MapGrapher::write_map(const char* map_name) const {
	ofstream output;
	open_for_writing(&output, (string("maps") + PATH_SEP + map_name + ".mgf").c_str(), true);
	m_graph->write(&output);
	bool failed = !output.bad();
	output.close();
	return failed;
}

SparseIntersectMap* MapGrapher::get_graph() {
	return m_graph;
}
