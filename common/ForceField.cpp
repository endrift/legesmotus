/*
 * common/ForceField.cpp
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

#include "ForceField.hpp"
#include "ClientMapObject.hpp"
#include "MapReader.hpp"
#include <cstring>
#include <Box2D/Box2D.h>
#include <iostream>
#include "misc.hpp"
#include "math.hpp"
#include "common/team.hpp"
#include "common/Player.hpp"

using namespace LM;
using namespace std;

ForceField::ForceField(Point position, ClientMapObject* clientpart) : MapObject(position, clientpart) {
	m_bounding_shape = NULL;
	
	m_is_hazardous = false;
	m_team = 0;
	m_damage = 0;
	m_damage_rate = 100;
	
	// TODO: Make this sent in by game parameters from client.
	m_freeze_time = 10000;
	
	m_last_damage_time = 0;
	
	m_force = 1;
	m_force_dir = 0;
	m_friction = 0;
	
	// TODO: This needs a map file format change so we can set it to a value!
	m_decay = 0;
	
	m_is_directional = false;
	m_is_repulsive = true;
	m_affects_frozen = false;
	m_affects_thawed = false;
	m_sucks_off_obstacles = false;
}

void ForceField::interact(PhysicsObject* other, b2Contact* contact) {
	if (other->get_type() != PhysicsObject::PLAYER) {
		return;
	}
	
	b2WorldManifold manifold;
	contact->GetWorldManifold(&manifold);

	Player* player = static_cast<Player*>(other);
	
	if (m_team != 0 && player->get_team() == m_team) {
		return;
	}
	
	if ((player->is_frozen() && m_affects_frozen) || (!player->is_frozen() && m_affects_thawed)) {
		if (m_sucks_off_obstacles) {
			player->set_is_grabbing_obstacle(false);
		}
	
		float force = m_force - (m_decay * (player->get_position() - get_position()).get_magnitude());
		if (force < 0) {
			force = 0;
		}
	
		if (m_is_directional) {
			player->apply_force(b2Vec2(force * cos(m_force_dir), force * sin(m_force_dir)));
		} else {
			Point anglevec = (player->get_position() - get_position()).get_unit_vector();
			anglevec *= m_is_repulsive ? 1 : -1;
			player->apply_force(b2Vec2(force * anglevec.x, force * anglevec.y));
		}
	}
	
	if (!m_is_hazardous) {
		return;
	}
	
	if (!player->is_frozen() && m_damage != 0 && m_last_damage_time < get_ticks() - m_damage_rate) {
			
		if (m_last_damage_time == 0) {
			m_last_damage_time = get_ticks()-m_damage_rate;
		}
		
		player->change_energy(-1 * m_damage * ((get_ticks() - m_last_damage_time)/m_damage_rate));
		m_last_damage_time = get_ticks();

		// Freeze 'em if they're dead.
		if (player->get_energy() == 0) {
			player->set_is_frozen(true, m_freeze_time);
			m_last_damage_time = 0;
		}
	}
}

void ForceField::initialize_physics(b2World* world) {
	b2BodyDef body_def;
	body_def.type = b2_staticBody;
	body_def.angle = to_radians(get_rotation());
	body_def.position.Set(to_physics(get_position().x), to_physics(get_position().y)); // 
	
	m_physics_body = world->CreateBody(&body_def);
	
	m_physics_body->SetUserData((void*)this);
	
	if (m_bounding_shape == NULL) {
		WARN("No bounding shape for ForceField that should have physics.");
		return;
	}
	
	m_physics_body->CreateFixture(m_bounding_shape, 0.0f);
}

void ForceField::init(MapReader* reader) {
	MapObject::init(reader);

	string bounding_shape;

	(*reader) >> bounding_shape;

	while (reader->has_more()) {
		string param_string;
		(*reader) >> param_string;

		if (strncmp(param_string.c_str(), "team=", 5) == 0) {
			m_team = parse_team_string(param_string.c_str() + 5);
		} else if (strncmp(param_string.c_str(), "damage=", 7) == 0) {
			m_damage = atoi(param_string.c_str() + 7);
			m_is_hazardous = true;
		} else if (strncmp(param_string.c_str(), "rate=", 5) == 0) {
			m_damage_rate = atol(param_string.c_str() + 5);
		} else if (strncmp(param_string.c_str(), "strength=", 9) == 0) {
			m_force = atof(param_string.c_str() + 9);
			m_is_repulsive = true;
			if (m_force < 0) {
				m_is_repulsive = false;
				m_force = -1 * m_force;
			}
			m_is_directional = false;
		} else if (strncmp(param_string.c_str(), "force=", 6) == 0) {
			Vector forcevec = Vector::make_from_string(param_string.c_str() + 6);
			m_force = forcevec.get_magnitude();
			m_force_dir = forcevec.get_angle();
			m_is_directional = true;
			m_is_repulsive = false;
		} else if (strncmp(param_string.c_str(), "friction=", 9) == 0) {
			m_friction = atof(param_string.c_str() + 9);
		} else if (strncmp(param_string.c_str(), "gravity=", 8) == 0) {
			m_force = atof(param_string.c_str() + 8);
			m_is_repulsive = false;
			if (m_force < 0) {
				m_is_repulsive = true;
				m_force = -1 * m_force;
			}
			m_is_directional = false;
		} else if (param_string == "nofrozen") {
			m_affects_frozen = false;
		} else if (param_string == "frozen") {
			m_affects_frozen = true;
		} else if (param_string == "nothawed") {
			m_affects_thawed = false;
		} else if (param_string == "thawed") {
			m_affects_thawed = true;
		} else if (param_string == "sucks-off-obstacles") {
			m_sucks_off_obstacles = true;
		} else {
			parse_param(param_string.c_str());
		}
	}

	if (m_bounding_shape != NULL) {
		delete m_bounding_shape;
	}

	m_bounding_shape = make_bounding_shape(bounding_shape, get_position());
}
