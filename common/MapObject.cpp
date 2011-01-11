/*
 * common/MapObject.cpp
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

#include "ClientMapObject.hpp"
#include "MapObject.hpp"
#include "MapReader.hpp"
#include "Polygon.hpp"
#include "common/Point.hpp"
#include <memory>
#include <cstring>
#include "common/physics.hpp"
#include <vector>
#include "common/math.hpp"

using namespace LM;
using namespace std;

MapObject::MapObject(Point position, ClientMapObject* clientpart) {
	m_position = position;
	m_clientpart = clientpart;

	m_is_tiled = false;
	m_scale_x = 1;
	m_scale_y = 1;
	m_rotation = 0;
}

MapObject::~MapObject() {
	delete m_clientpart;
}

bool MapObject::parse_param(const char* option_string) {
	if (strncmp(option_string, "tile=", 5) == 0) {
		set_is_tiled(true);
		set_tile_dimensions(Vector::make_from_string(option_string + 5));
	} else if (strcmp(option_string, "notile") == 0) {
		set_is_tiled(false);
	} else if (strncmp(option_string, "scale=", 6) == 0) {
		set_scale_x(atof(option_string + 6));
		set_scale_y(atof(option_string + 6));
	} else if (strncmp(option_string, "rotate=", 7) == 0) {
		set_rotation(atof(option_string + 7));
	} else {
		return false;
	}

	return true;
}

void MapObject::set_position(Point position) {
	m_position = position;
	if (m_clientpart != NULL) {
		m_clientpart->set_position(position);
	}
}

void MapObject::set_is_tiled(bool is_tiled) {
	m_is_tiled = is_tiled;
	if (m_clientpart != NULL) {
		m_clientpart->set_is_tiled(is_tiled);
	}
}

void MapObject::set_tile_dimensions(Vector tile_dimensions) {
	m_tile_dimensions = tile_dimensions;
	if (m_clientpart != NULL) {
		m_clientpart->set_tile_dimensions(tile_dimensions);
	}
}

void MapObject::set_scale_x(float scale_x) {
	m_scale_x = scale_x;
	if (m_clientpart != NULL) {
		m_clientpart->set_scale_x(scale_x);
	}
}

void MapObject::set_scale_y(float scale_y) {
	m_scale_y = scale_y;
	if (m_clientpart != NULL) {
		m_clientpart->set_scale_y(scale_y);
	}
}

void MapObject::set_rotation(float rotation) {
	m_rotation = rotation;
	if (m_clientpart != NULL) {
		m_clientpart->set_rotation(rotation);
	}
}

ClientMapObject* MapObject::get_client_part() {
	return m_clientpart;
}

b2Shape* MapObject::make_bounding_shape(const std::string& shape_string) {
	if (m_is_tiled) {
		b2PolygonShape* shape = new b2PolygonShape();
		
		// If the object is being tiled, we ignore the specified bounding shape and
		// use a rectangle of the tile dimensions instead.
		shape->SetAsBox(to_physics(m_tile_dimensions.x/2 * m_scale_x), to_physics(m_tile_dimensions.y/2 * m_scale_y),
		                b2Vec2(to_physics(m_tile_dimensions.x/2 * m_scale_x), to_physics(m_tile_dimensions.y/2 * m_scale_y)), 0);
		return shape;
	} else {
		return make_shape_from_string(shape_string, m_scale_x, m_scale_y);
	}
}

void MapObject::init(MapReader* reader) {
	if (m_clientpart != NULL) {
		m_clientpart->read(reader, this);
	}
}
