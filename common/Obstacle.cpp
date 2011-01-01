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
#include "misc.hpp"
#include "math.hpp"
#include "common/team.hpp"
#include "common/Player.hpp"

using namespace LM;
using namespace std;

Obstacle::Obstacle(Point position, ClientMapObject* clientpart) : MapObject(position, clientpart) {
	// TODO
	m_is_slippery = false;
	m_is_collidable = true;
	m_bounce_factor = 0.9;
	m_bounding_shape = NULL;
	
	m_is_hazardous = false;
	m_team = 0;
	m_damage = 0;
	m_damage_rate = 100;
	m_collision_damage = 0;
	
	// TODO: Make this sent in by game parameters from client.
	m_freeze_time = 10000;
	m_freeze_on_hit = 0;
	m_repel_velocity = 100.0;
	
	m_last_damage_time = 0;
}

MapObject::CollisionResult Obstacle::get_collision_result(PhysicsObject* other, b2Contact* contact) {
	if (!m_is_collidable) {
		return IGNORE;
	}

	if (m_is_slippery) {
		return BOUNCE;
	} else {
		return GRAB;
	}
}

MapObject::CollisionResult Obstacle::collide(PhysicsObject* other, b2Contact* contact) {
	// Check if we're hazardous and the player is affected:
	if (other->get_type() == PhysicsObject::PLAYER && m_is_hazardous) {
		b2WorldManifold manifold;
		contact->GetWorldManifold(&manifold);
		b2Vec2 repel_normal = manifold.normal;
	
		Player* player = static_cast<Player*>(other);
		
		if (m_team == 0 || player->get_team() != m_team && !player->is_frozen()) {
			if (m_freeze_on_hit != 0) {
				player->set_is_frozen(true, m_freeze_on_hit);
				player->apply_force(b2Vec2(m_repel_velocity * repel_normal.x, m_repel_velocity * repel_normal.y));
			}
		
			if (m_collision_damage != 0) {
				player->change_energy(-1 * m_collision_damage);
				m_last_damage_time = get_ticks();
		
				// Freeze 'em if they're dead.
				if (player->get_energy() == 0) {
					player->set_is_frozen(true, m_freeze_time);
					player->apply_force(b2Vec2(m_repel_velocity * repel_normal.x, m_repel_velocity * repel_normal.y));
				}
			}
		}
	}
	
	return get_collision_result(other, contact);
}

void Obstacle::interact(PhysicsObject* other, b2Contact* contact) {
	if (other->get_type() != PhysicsObject::PLAYER || !m_is_hazardous) {
		return;
	}
	
	b2WorldManifold manifold;
	contact->GetWorldManifold(&manifold);
	b2Vec2 repel_normal = manifold.normal;

	Player* player = static_cast<Player*>(other);
	
	if ((m_team == 0 || player->get_team() != m_team) && !player->is_frozen() && m_damage != 0 &&
			m_last_damage_time < get_ticks() - m_damage_rate) {
			
		if (m_last_damage_time == 0) {
			m_last_damage_time = get_ticks()-m_damage_rate;
		}
		
		player->change_energy(-1 * m_damage * ((get_ticks() - m_last_damage_time)/m_damage_rate));
		m_last_damage_time = get_ticks();

		// Freeze 'em if they're dead.
		if (player->get_energy() == 0) {
			player->set_is_frozen(true, m_freeze_time);
			player->apply_force(b2Vec2(m_repel_velocity * repel_normal.x, m_repel_velocity * repel_normal.y));
		}
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
		WARN("No bounding shape for obstacle that should have physics.");
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
		} else if (strncmp(param_string.c_str(), "team=", 5) == 0) {
			m_team = parse_team_string(param_string.c_str() + 5);
		} else if (param_string == "collidable") {
			m_is_collidable = true;
		} else if (param_string == "uncollidable") {
			m_is_collidable = false;
		} else if (strncmp(param_string.c_str(), "collision_damage=", 17) == 0) {
			m_collision_damage = atoi(param_string.c_str() + 17);
			m_is_hazardous = true;
		} else if (strncmp(param_string.c_str(), "damage=", 7) == 0) {
			m_damage = atoi(param_string.c_str() + 7);
			m_is_hazardous = true;
		} else if (strncmp(param_string.c_str(), "rate=", 5) == 0) {
			m_damage_rate = atol(param_string.c_str() + 5);
		} else if (strncmp(param_string.c_str(), "freeze=", 7) == 0) {
			m_freeze_on_hit = atol(param_string.c_str() + 7);
			m_is_hazardous = true;
		} else if (strncmp(param_string.c_str(), "repel=", 6) == 0) {
			m_repel_velocity = atof(param_string.c_str() + 6);
		} else {
			parse_param(param_string.c_str());
		}
	}

	if (m_bounding_shape != NULL) {
		delete m_bounding_shape;
	}

	m_bounding_shape = make_bounding_shape(bounding_shape, get_position());
}
