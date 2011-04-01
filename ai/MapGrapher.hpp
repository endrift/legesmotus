/*
 * ai/MapGrapher.hpp
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

#ifndef LM_AI_MAPGRAPHER_HPP
#define LM_AI_MAPGRAPHER_HPP

#include <list>
#include <string>
#include "common/RayCast.hpp"

class b2World;
class b2Body;

namespace LM {
	class SparseIntersectMap;
	class Map;
	class MapObject;
	class PhysicsObject;
	class GameLogic;

	class MapGrapher {

	private:
		const static int GRANULARITY;
		const static int MAX_SIZE;
		const static int BUFFER_ANGLE;
		const static float MULTI_CAST_WIDTH;
		
		const static bool MULTI_CAST;
		
		float dist_change;
		float theta_change;
		
		long int objects_mapped;
		long int sides_mapped;
		long int entries_mapped;
		long int entries_skipped;
		
		uint64_t start_time;
	
		SparseIntersectMap* m_graph;
		b2World* m_physics;
		std::list<b2Body*> m_objects;
		RayCast m_ray_cast;
		RayCast m_ray_cast2;
		RayCast m_ray_cast3;
		
		int m_width;
		int m_height;
		
		void map_object(b2Body* object);
		void map_segment(b2Vec2 start, b2Vec2 end, PhysicsObject* obj = NULL);

	public:
		MapGrapher();
		~MapGrapher();
		
		void load_map(const GameLogic* logic, b2World* world);
		
		// If a number n of objects is passed, will only map the next n bodies.
		void do_mapping(int num_objects = -1);
		
		bool is_done_mapping() const;
		
		bool write_map(const char* map_name) const;
		
		SparseIntersectMap* get_graph();
	};
}

#endif
