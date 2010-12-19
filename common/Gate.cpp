/*
 * common/Gate.cpp
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

#include "MapReader.hpp"
#include "Gate.hpp"
#include "common/team.hpp"
#include <string>
#include <cstring>
#include <cstdlib>
#include <Box2D/Box2D.h>
#include "common/math.hpp"
#include "common/misc.hpp"
#include "common/ClientMapObject.hpp"

using namespace LM;
using namespace std;

Gate::Gate(Point position, ClientMapObject* clientpart) : MapObject(position, clientpart) {
	m_team = 0;
	m_width = 8;
	m_length = 109;
	m_extent = 24;
	m_progress = 0.0;

	m_bounding_shape = NULL;
}

void Gate::initialize_physics(b2World* world) {
	b2BodyDef body_def;
	// TODO: Currently, all map objects are static; make this configured
	body_def.type = b2_staticBody;
	body_def.angle = to_radians(get_rotation());
	body_def.position.Set(to_physics(get_position().x), to_physics(get_position().y)); // 
	
	m_physics_body = world->CreateBody(&body_def);
	
	m_physics_body->SetUserData((void*)this);
	
	if (m_bounding_shape == NULL) {
		WARN("No bounding shape for obstacle that should have physics.");
		return;
	}
	
	m_physics_body->CreateFixture(m_bounding_shape, 0.0f);
}

void Gate::init(MapReader* reader) {
	MapObject::init(reader);

	get_client_part()->set_is_tiled(true);

	while (reader->has_more()) {
		string param_string;
		(*reader) >> param_string;

		if (strncmp(param_string.c_str(), "team=", 5) == 0) {
			m_team = parse_team_string(param_string.c_str() + 5);
		} else if (strncmp(param_string.c_str(), "width=", 6) == 0) {
			m_width = atof(param_string.c_str() + 6);
			get_client_part()->set_scale_x(m_width);
		} else if (strncmp(param_string.c_str(), "length=", 7) == 0) {
			m_length = atof(param_string.c_str() + 7);
			get_client_part()->set_scale_y(m_length);
		} else if (strncmp(param_string.c_str(), "extent=", 7) == 0) {
			m_extent = atof(param_string.c_str() + 7);
		} else if (strncmp(param_string.c_str(), "rotate=", 7) == 0) {
			m_rotation = atof(param_string.c_str() + 7);
		} 
	}
	
	DEBUG("Gate: " << get_client_part() << endl);

	if (m_bounding_shape != NULL) {
		delete m_bounding_shape;
	}
	
	b2Vec2 size(m_width + m_extent * 2.0, m_length);
	b2PolygonShape* shape = new b2PolygonShape();
	shape->SetAsBox(to_physics(size.x/2 * get_scale_x()), to_physics(size.y/2 * get_scale_y()),
		b2Vec2(to_physics(size.x/2 * get_scale_x()), to_physics(size.y/2 * get_scale_y())), 0);

	m_bounding_shape = shape;
}
