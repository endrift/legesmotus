/*
 * newclient/GameLogic.cpp
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

#include "GameLogic.hpp"
#include "common/Map.hpp"
#include <Box2D/Box2D.h>
#include <iostream>
#include "common/math.hpp"
#include "common/Weapon.hpp"

using namespace LM;
using namespace std;

GameLogic::GameLogic(Map* map) {
	srand(time(NULL));
	m_map = map;
	m_physics = NULL;

	b2Vec2 gravity(0.0f, 0.0f);
	m_physics = new b2World(gravity, true);
	
	m_physics->SetContactListener(this);
	
	m_weapons.clear();
}

GameLogic::~GameLogic() {
	for (map<uint32_t, Player*>::iterator iter = m_players.begin(); iter != m_players.end(); ++iter) {
		delete iter->second;
	}
	delete m_map;
	
	clear_weapons();
	
	delete m_physics;
}

void GameLogic::update_map() {
	m_map->initialize_physics(m_physics);
}

Map* GameLogic::get_map() {
	return m_map;
}

void GameLogic::add_player(Player* player) {
	// TODO: Put them at their proper spawn location.
	//player->set_position(50, 50);
	player->initialize_physics(m_physics);
	
	// TODO: Testing code for physics - remove later.
	//player->apply_force(b2Vec2(200.0f, 50.0f));
	
	m_players[player->get_id()] = player;
}

void GameLogic::remove_player(uint32_t id) {
	Player* player = m_players[id];
	m_players.erase(id);
	delete player;
}

void GameLogic::add_weapon(size_t index, Weapon* weapon) {
	m_weapons.insert(std::make_pair(weapon->get_id(), weapon));
}

void GameLogic::clear_weapons() {
	for (map<string, Weapon*>::iterator it(m_weapons.begin()); it != m_weapons.end(); ++it) {
		delete it->second;
	}
	m_weapons.clear();
}

void GameLogic::step() {
	m_physics->Step(PHYSICS_TIMESTEP, VEL_ITERATIONS, POS_ITERATIONS);
	
	// Remove any forces we applied for this timestep.
	m_physics->ClearForces();
	
	for (map<uint32_t, Player*>::iterator iter = m_players.begin(); iter != m_players.end(); ++iter) {
		Player* player = iter->second;
		player->update_physics();
		
		// Create any contact joints we need to have.		
		while (m_joints_to_create.size() > 0) {
			pair<b2Body*, b2JointDef*> this_joint_pair = m_joints_to_create.back();
			create_contact_joint(this_joint_pair.first, this_joint_pair.second);
			m_joints_to_create.pop_back();
		}

		//b2Body* body = player->get_physics_body();
		//b2Vec2 position = body->GetPosition();
		//float32 angle = body->GetAngle();

		//cerr << position.x << ", " << position.y << ", " << angle << endl;
	}
}

Player* GameLogic::get_player(const uint32_t id) {
	return m_players[id];
}

Weapon*	GameLogic::get_weapon(const string& name) {
	map<string, Weapon*>::iterator it(m_weapons.find(name));
	return it == m_weapons.end() ? NULL : it->second;
}

b2World* GameLogic::get_world() {
	return m_physics;
}

void GameLogic::attempt_jump(uint32_t player_id, float angle) {
	if (m_players[player_id]->is_grabbing_obstacle() && !m_players[player_id]->is_frozen()) {
		m_players[player_id]->set_is_grabbing_obstacle(false);
	
		m_players[player_id]->apply_force(b2Vec2(JUMP_STRENGTH * cos(angle), JUMP_STRENGTH  * sin(angle)));
		m_players[player_id]->apply_torque(-1*(JUMP_ROTATION/2.0f) + (float)rand()/(float)RAND_MAX * JUMP_ROTATION);
	}
}

bool GameLogic::attempt_fire(uint32_t player_id, std::string weapon_id, float angle) {
	Weapon* weapon = get_weapon(weapon_id);
	Player* player = get_player(player_id);
	
	if (weapon == NULL) {
		return false;
	}
	
	if (player->is_frozen() || weapon->get_remaining_cooldown() > 0) {
		return false;
	}
	
	weapon->fire(m_physics, *player, player->get_position(), angle);
	return true;
}

void GameLogic::BeginContact(b2Contact* contact) {
	b2Fixture* fixture1 = contact->GetFixtureA();
	b2Fixture* fixture2 = contact->GetFixtureB();
	
	b2Body* body1 = fixture1->GetBody();
	b2Body* body2 = fixture2->GetBody();
	
	b2WorldManifold manifold;
	contact->GetWorldManifold(&manifold);
	
	//b2Manifold* local_manifold = contact->GetManifold();
	
	if (body1->GetUserData() == NULL || body2->GetUserData() == NULL) {
		cerr << "Body has no user data!" << endl;
		return;
	}
	
	PhysicsObject* userdata1 = static_cast<PhysicsObject*>(body1->GetUserData());
	PhysicsObject* userdata2 = static_cast<PhysicsObject*>(body2->GetUserData());
	
	// If we have a collision between a player and a map object, we need to create a joint to revolve around
	if (userdata1->get_type() == PhysicsObject::PLAYER && (userdata2->get_type() == PhysicsObject::MAP_OBJECT || userdata2->get_type() == PhysicsObject::MAP_EDGE)) {
		Player* player = static_cast<Player*>(userdata1);
		if (!player->is_grabbing_obstacle() && !player->is_frozen()) {
			b2RevoluteJointDef* joint_def = new b2RevoluteJointDef;
			joint_def->Initialize(body1, body2, manifold.points[0]); // body1->GetWorldPoint(local_manifold->localPoint)
			joint_def->collideConnected = true;
			joint_def->maxMotorTorque = 5.0f;
			joint_def->motorSpeed = 0.0f;
			joint_def->enableMotor = false;
	
			m_joints_to_create.push_back(pair<b2Body*, b2JointDef*>(body1, joint_def));
	
			contact->SetEnabled(true);
		}
	} else if ((userdata1->get_type() == PhysicsObject::MAP_OBJECT || userdata1->get_type() == PhysicsObject::MAP_EDGE) && userdata2->get_type() == PhysicsObject::PLAYER) {
		Player* player = static_cast<Player*>(userdata2);
		if (!player->is_grabbing_obstacle() && !player->is_frozen()) {
			b2RevoluteJointDef* joint_def = new b2RevoluteJointDef;
			joint_def->Initialize(body1, body2, manifold.points[0]); // body2->GetWorldPoint(local_manifold->localPoint)
			joint_def->collideConnected = true;
			joint_def->maxMotorTorque = 5.0f;
			joint_def->motorSpeed = 0.0f;
			joint_def->enableMotor = false;
		
			m_joints_to_create.push_back(pair<b2Body*, b2JointDef*>(body2, joint_def));
	
			contact->SetEnabled(true);
		}
	}
}

void GameLogic::create_contact_joint(b2Body* body1, b2JointDef* joint_def) {
	PhysicsObject* userdata = static_cast<PhysicsObject*>(body1->GetUserData());

	b2Joint* joint = m_physics->CreateJoint(joint_def);
	Player* player = static_cast<Player*>(userdata);

	player->set_attach_joint(joint);
	
	delete joint_def;
}

void GameLogic::EndContact(b2Contact* contact) {
}

void GameLogic::PreSolve(b2Contact* contact, const b2Manifold* old_manifold) {
}

float GameLogic::get_dist(b2Vec2 point1, b2Vec2 point2) {
	return sqrt((point2.y - point1.y)*(point2.y-point1.y) + (point2.x - point1.x));
}

void GameLogic::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) {
b2Fixture* fixture1 = contact->GetFixtureA();
	b2Fixture* fixture2 = contact->GetFixtureB();
	
	b2Body* body1 = fixture1->GetBody();
	b2Body* body2 = fixture2->GetBody();
	
	b2WorldManifold manifold;
	contact->GetWorldManifold(&manifold);
	
	if (body1->GetUserData() == NULL || body2->GetUserData() == NULL) {
		cerr << "Body has no user data!" << endl;
		return;
	}
	
	PhysicsObject* userdata1 = static_cast<PhysicsObject*>(body1->GetUserData());
	PhysicsObject* userdata2 = static_cast<PhysicsObject*>(body2->GetUserData());

	// If the impulse is non-zero on the second point in the collision, then we have hit
	// the wall with two corners. This means we should weld to it.
	b2Manifold* localmanifold = contact->GetManifold();
	if (localmanifold->pointCount > 1 && impulse->normalImpulses[1] > 0) {
		if (contact->IsEnabled() && userdata1->get_type() == PhysicsObject::PLAYER && 
				(userdata2->get_type() == PhysicsObject::MAP_OBJECT || 
				userdata2->get_type() == PhysicsObject::MAP_EDGE)) {
			Player* player = static_cast<Player*>(userdata1);
			if (!player->is_frozen() && (!player->is_grabbing_obstacle() || (player->get_attach_joint() != NULL &&
					player->get_attach_joint()->GetType() != e_weldJoint))) {
				b2WeldJointDef* joint_def = new b2WeldJointDef;
				joint_def->Initialize(body1, body2, manifold.points[0]);
				joint_def->collideConnected = true;
		
				m_joints_to_create.insert(m_joints_to_create.begin(), pair<b2Body*, b2JointDef*>(body1, joint_def));
		
				contact->SetEnabled(false);
			}
		} else if (contact->IsEnabled() && (userdata1->get_type() == PhysicsObject::MAP_OBJECT || 
				userdata1->get_type() == PhysicsObject::MAP_EDGE) && 
				userdata2->get_type() == PhysicsObject::PLAYER) {
			Player* player = static_cast<Player*>(userdata2);
			if (!player->is_frozen() && (!player->is_grabbing_obstacle() || (player->get_attach_joint() != NULL &&
					player->get_attach_joint()->GetType() != e_weldJoint))) {
				b2WeldJointDef* joint_def = new b2WeldJointDef;
				joint_def->Initialize(body1, body2, manifold.points[0]);
				joint_def->collideConnected = true;
		
				m_joints_to_create.insert(m_joints_to_create.begin(), pair<b2Body*, b2JointDef*>(body2, joint_def));
		
				contact->SetEnabled(false);
			}
		}
	}
}
