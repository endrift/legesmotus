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
	
	////////
	
	m_map->initialize_physics(m_physics);
}

void GameLogic::add_player(Player* player) {
	// TODO: Put them at their proper spawn location.
	player->set_position(50, 50);
	player->initialize_physics(m_physics);
	
	// TODO: Testing code for physics - remove later.
	player->get_physics_body()->ApplyForce(b2Vec2(400.0f, 30.0f), player->get_physics_body()->GetWorldCenter());
	
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
