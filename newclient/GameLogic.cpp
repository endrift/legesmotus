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

using namespace LM;
using namespace std;

GameLogic::GameLogic(Map* map) {
	m_map = map;
	m_physics = NULL;

	update_map(map);

}

GameLogic::~GameLogic() {
	for (map<uint32_t, Player*>::iterator iter = m_players.begin(); iter != m_players.end(); ++iter) {
		delete iter->second;
	}
	delete m_map;
	
	delete m_physics;
}

void GameLogic::update_map(Map* map) {
	m_map = map;
	
	//////// PHYSICS INIT
	// TODO: Do we want to have this here, or somewhere else?
	
	if (m_physics != NULL) {
		delete m_physics;
	}
	
	b2Vec2 gravity(0.0f, 0.0f);
	bool doSleep = true;
	m_physics = new b2World(gravity, doSleep);
	
	m_physics->SetContactListener(this);
	
	////////
	
	m_map->initialize_physics(m_physics);
}

void GameLogic::add_player(Player* player) {
	// TODO: Put them at their proper spawn location.
	player->set_position(50, 50);
	player->initialize_physics(m_physics);
	
	// TODO: Testing code for physics - remove later.
	player->apply_force(b2Vec2(300.0f, 210.0f));
	
	m_players[player->get_id()] = player;
}

void GameLogic::remove_player(uint32_t id) {
	Player* player = m_players[id];
	m_players[id] = NULL;
	delete player;
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

Player* GameLogic::get_player(uint32_t id) {
	return m_players[id];
}

b2World* GameLogic::get_world() {
	return m_physics;
}

void GameLogic::attempt_jump(uint32_t player_id, float angle) {
	if (m_players[player_id]->is_grabbing_obstacle()) {
		m_players[player_id]->set_is_grabbing_obstacle(false);
	
		m_players[player_id]->apply_force(b2Vec2(JUMP_STRENGTH * cos(angle), JUMP_STRENGTH  * sin(angle)));
	}
}

void GameLogic::BeginContact(b2Contact* contact) {
	b2Fixture* fixture1 = contact->GetFixtureA();
	b2Fixture* fixture2 = contact->GetFixtureB();
	
	b2Body* body1 = fixture1->GetBody();
	b2Body* body2 = fixture2->GetBody();
	
	b2WorldManifold manifold;
	contact->GetWorldManifold(&manifold);
	
	b2Manifold* local_manifold = contact->GetManifold();
	
	if (body1->GetUserData() == NULL || body2->GetUserData() == NULL) {
		cerr << "Body has no user data!" << endl;
		return;
	}
	
	PhysicsObject* userdata1 = static_cast<PhysicsObject*>(body1->GetUserData());
	PhysicsObject* userdata2 = static_cast<PhysicsObject*>(body2->GetUserData());
	
	if (userdata1->get_type() == PhysicsObject::PLAYER && (userdata2->get_type() == PhysicsObject::MAP_OBJECT || userdata2->get_type() == PhysicsObject::MAP_EDGE)) {
		Player* player = static_cast<Player*>(userdata1);
		if (!player->is_grabbing_obstacle()) {
			b2RevoluteJointDef* joint_def = new b2RevoluteJointDef;
			joint_def->Initialize(body1, body2, manifold.points[0]); // body1->GetWorldPoint(local_manifold->localPoint)
			joint_def->collideConnected = true;
			joint_def->maxMotorTorque = 5.0f;
			joint_def->motorSpeed = 0.0f;
			joint_def->enableMotor = true;
		
			m_joints_to_create.push_back(pair<b2Body*, b2JointDef*>(body1, joint_def));
		
			contact->SetEnabled(true);
		}
	} else if ((userdata1->get_type() == PhysicsObject::MAP_OBJECT || userdata1->get_type() == PhysicsObject::MAP_EDGE) && userdata2->get_type() == PhysicsObject::PLAYER) {
		Player* player = static_cast<Player*>(userdata1);
		if (!player->is_grabbing_obstacle()) {
			b2RevoluteJointDef* joint_def = new b2RevoluteJointDef;
			joint_def->Initialize(body1, body2, manifold.points[0]); // body2->GetWorldPoint(local_manifold->localPoint)
			joint_def->collideConnected = true;
			joint_def->maxMotorTorque = 5.0f;
			joint_def->motorSpeed = 0.0f;
			joint_def->enableMotor = true;
			
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

void GameLogic::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) {
}
