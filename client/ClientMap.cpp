/*
 * client/ClientMap.cpp
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

#include "ClientMap.hpp"
#include "ClientGate.hpp"
#include "BaseMapObject.hpp"
#include "MapObjectParams.hpp"
#include "common/MapReader.hpp"
#include "common/Shape.hpp"
#include "common/Polygon.hpp"
#include "common/team.hpp"
#include <cstring>
#include <memory>

using namespace LM;
using namespace std;

ClientMap::ClientMap() {
	m_gates[0] = m_gates[1] = NULL;
}

ClientMap::~ClientMap() {
	clear();
}

void	ClientMap::clear() {
	for (list<BaseMapObject*>::iterator it(m_objects.begin()); it != m_objects.end(); ++it) {
		if ((*it)->has_graphic()) {
			unregister_graphic((*it)->get_graphic());
		}
		delete *it;
	}
	m_objects.clear();
	m_gates[0] = m_gates[1] = NULL;
	Map::clear();
}

void	ClientMap::add_object(MapReader& object_data) {
	if (BaseMapObject* new_object = BaseMapObject::make_map_object(object_data, *this)) {
		m_objects.push_back(new_object);
	}
}

// progress is in [0.0,1.0], where 0 == closed .. 1 == fully open
void	ClientMap::set_gate_progress(char team, double progress) {
	if (ClientGate* gate = get_gate_object(team)) {
		gate->set_progress(*this, progress);
	}
}

ClientGate* 	ClientMap::get_gate_object(char team) {
	if (is_valid_team(team)) {
		return m_gates[team - 'A'];
	} else {
		return NULL;
	}
}

void	ClientMap::reset_gates() {
	set_gate_progress('A', 0.0);
	set_gate_progress('B', 0.0);
}

void	ClientMap::reset() {
	reset_gates();
}

Shape*	ClientMap::make_bounding_shape(const std::string& shape_string, Point position, const MapObjectParams& params) {
	std::auto_ptr<Shape>	shape;

	if (params.is_tiled) {
		// If the object is being tiled, we ignore the specified bounding shape and
		// use a rectangle of the tile dimensions instead.
		Polygon*	poly = new Polygon(position);
		shape.reset(poly);
		poly->make_rectangle(params.tile_dimensions.x, params.tile_dimensions.y);
	} else {
		shape.reset(Shape::make_from_string(shape_string.c_str(), position));

		if (!shape.get()) {
			return NULL;
		}
	}

	shape->rotate(params.rotation);
	shape->scale(params.scale_x); // TODO: support scaling of shapes in both x and y directions

	return shape.release();
}

void	ClientMap::register_gate(char team, ClientGate* gate) {
	if (is_valid_team(team)) {
		m_gates[team - 'A'] = gate;
	}
}

