/*
 * common/Obstacle.cpp
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

#include "Obstacle.hpp"
#include "ClientMapObject.hpp"
#include "MapReader.hpp"
#include <cstring>
#include <Box2D/Box2D.h>
#include <iostream>
#include "math.hpp"

using namespace LM;
using namespace std;

Obstacle::Obstacle(Point position, ClientMapObject* clientpart) : MapObject(position, clientpart) {
	// TODO
	m_is_slippery = false;
	m_bounce_factor = 0.9;
	m_bounding_shape = NULL;
}

MapObject::CollisionResult Obstacle::collide(GameLogic* logic, PhysicsObject* other, b2Contact* contact) {
	if (m_is_slippery) {
		return BOUNCE;
	} else {
		return GRAB;
	}
}

void Obstacle::initialize_physics(b2World* world) {
	b2BodyDef body_def;
	// TODO: Currently, all map objects are static; make this configured
	body_def.type = b2_staticBody;
	body_def.angle = to_radians(get_rotation());
	body_def.position.Set(to_physics(get_position().x), to_physics(get_position().y)); // 
	
	m_physics_body = world->CreateBody(&body_def);
	
	m_physics_body->SetUserData((void*)this);
	
	if (m_bounding_shape == NULL) {
		cerr << "No bounding shape for obstacle that should have physics." << endl;
		return;
	}
	
	m_physics_body->CreateFixture(m_bounding_shape, 0.0f);
}

void Obstacle::init(MapReader* reader) {
	MapObject::init(reader);

	string bounding_shape;

	(*reader) >> bounding_shape;

	while (reader->has_more()) {
		string param_string;
		(*reader) >> param_string;

		if (param_string == "slippery") {
			m_is_slippery = true;
		} else if (param_string == "sticky") {
			m_is_slippery = false;
		} else if (strncmp(param_string.c_str(), "bounce=", 7) == 0) {
			m_bounce_factor = atof(param_string.c_str() + 7);
		} else {
			parse_param(param_string.c_str());
		}
	}

	if (m_bounding_shape != NULL) {
		delete m_bounding_shape;
	}

	m_bounding_shape = make_bounding_shape(bounding_shape, get_position());
}
