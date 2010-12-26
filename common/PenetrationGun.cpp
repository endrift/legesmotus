/*
 * common/PenetrationGun.cpp
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

#include "common/PenetrationGun.hpp"
#include "common/Player.hpp"
#include "common/StringTokenizer.hpp"
#include "common/PacketWriter.hpp"
#include "common/timer.hpp"
#include "common/math.hpp"
#include "common/TypeWrapper.hpp"
#include "common/misc.hpp"
#include <algorithm>
#include <stdlib.h>
#include <iostream>
#include <Box2D/Box2D.h>

using namespace LM;
using namespace std;

PenetrationGun::PenetrationGun(const char* id, StringTokenizer& gun_data) : Weapon(id) {
	m_last_fired_time = 0;

	m_freeze_time = 10000;
	m_damage = 100;
	m_cooldown = 700;
	m_recoil = 1.5;
	m_inaccuracy = 0;
	m_is_continuous = false;
	m_total_ammo = 0;
	m_ammo_recharge = 500;
	m_last_fired_dir = 0;
	
	// TODO: FOR NOW, HARDCODE MAX RANGE.
	m_max_range = 10000;

	while (gun_data.has_more()) {
		parse_param(gun_data.get_next());
	}

	reset();
}

bool PenetrationGun::parse_param(const char* param_string) {
	if (strncmp(param_string, "freeze=", 7) == 0) {
		m_freeze_time = atol(param_string + 7);
	} else if (strncmp(param_string, "damage=", 7) == 0) {
		m_damage = atoi(param_string + 7);
	} else if (strncmp(param_string, "cooldown=", 9) == 0) {
		m_cooldown = atol(param_string + 9);
	} else if (strncmp(param_string, "recoil=", 7) == 0) {
		m_recoil = atof(param_string + 7);
	} else if (strncmp(param_string, "inaccuracy=", 11) == 0) {
		m_inaccuracy = to_radians(atof(param_string + 11));
	} else if (strcmp(param_string, "ammo=unlimited") == 0) {
		m_total_ammo = 0;
	} else if (strncmp(param_string, "ammo=", 5) == 0) {
		m_total_ammo = atoi(param_string + 5);
	} else if (strncmp(param_string, "ammo_recharge=", 14) == 0) {
		m_ammo_recharge = atol(param_string + 14);
	} else if (strcmp(param_string, "continuous") == 0) {
		m_is_continuous = true;
	} else if (strcmp(param_string, "notcontinuous") == 0) {
		m_is_continuous = false;
	} else if (strncmp(param_string, "penetrate_players=", 18) == 0) {
		m_penetrates_players = atof(param_string + 18);
	} else if (strncmp(param_string, "penetrate_walls=", 16) == 0) {
		m_penetrates_walls = atof(param_string + 16);
	} else if (strncmp(param_string, "max_penetration=", 16) == 0) {
		m_max_penetration = atof(param_string + 16);
	} else {
		return Weapon::parse_param(param_string);
	}
	return true;
}

void PenetrationGun::fire(b2World* physics, Player& player, Point start, double direction) {
	float endx = start.x + m_max_range * cos(direction);
	float endy = start.y + m_max_range * sin(direction);
	m_last_fired_time = get_ticks();
	m_hit_data.clear();
	
	player.apply_force(b2Vec2(m_recoil * 100 * cos(M_PI + direction), m_recoil * 100 * sin(M_PI + direction)));
	
	physics->RayCast(this, b2Vec2(to_physics(start.x), to_physics(start.y)), b2Vec2(to_physics(endx), to_physics(endy)));
	if (!m_hit_data.empty()) {
		m_objects_penetrated = 0;
		m_current_damage = m_damage;
		sort(m_hit_data.begin(), m_hit_data.end(), compare_hit_data);
		m_last_fired_dir = direction;
	} else {
		m_hit_data.clear();
	}
}

void PenetrationGun::hit(Player* hit_player, const Packet::PlayerHit* p) {
	string extradata = *p->extradata.item;
	stringstream s(extradata);
	
	float direction;
	float hit_point_x;
	float hit_point_y;
	float damage;
	
	s >> direction >> hit_point_x >> hit_point_y >> damage;

	// Apply force to the player.
	hit_player->apply_force(b2Vec2(m_recoil * 100 * cos(direction), m_recoil * 100 * sin(direction)), b2Vec2(hit_point_x, hit_point_y));

	// Do damage if the gun has effect and they're not frozen.
	if (p->has_effect && !hit_player->is_frozen()) {
		// Apply damage to the player.
		hit_player->change_energy(-damage);
		
		// If player is damaged sufficiently, freeze them.
		if (hit_player->get_energy() <= 0) {
			hit_player->set_is_frozen(true, m_freeze_time);
		}
	}
}

void PenetrationGun::reset() {
	m_last_fired_time = 0;
	m_current_ammo = m_total_ammo;
}

uint64_t PenetrationGun::get_remaining_cooldown() const {
	if (m_last_fired_time) {
		uint64_t	time_since_fire = get_ticks() - m_last_fired_time;
		if (time_since_fire < m_cooldown) {
			return m_cooldown - time_since_fire;
		}
	}
	return 0;
}

int PenetrationGun::get_current_ammo () const {
	if (m_last_fired_time && m_ammo_recharge) {
		// Take into account the ammo recharge that has occurred since last firing
		uint64_t	time_since_fire = get_ticks() - m_last_fired_time;
		return min<int>(m_total_ammo, m_current_ammo + time_since_fire / m_ammo_recharge);
	}
	return m_current_ammo;
}

Packet::PlayerHit* PenetrationGun::generate_next_hit_packet(Packet::PlayerHit* p, Player* shooter) {
	if (m_hit_data.empty()) {
		return NULL;
	}
	
	bool found_player = false;
	
	while (!found_player && !m_hit_data.empty()) {
		m_objects_penetrated++;
	
		// Don't go over max penetration depth.
		if (m_objects_penetrated > m_max_penetration) {
			return NULL;
		}
	
		if (m_current_damage <= 0) {
			return NULL;
		}
	
		HitData nextdata = m_hit_data.back();
		m_hit_data.pop_back();

		b2Body* body = nextdata.fixture->GetBody();
	
		PhysicsObject* userdata = static_cast<PhysicsObject*>(body->GetUserData());
		if (userdata->get_type() != PhysicsObject::PLAYER) {
			m_current_damage = m_current_damage * m_penetrates_walls;
			continue;
		}

		p->shooter_id = shooter->get_id();
		p->weapon_id = get_id();

		Player* hit_player = static_cast<Player*>(userdata);

		p->shot_player_id = hit_player->get_id();
		p->has_effect = hit_player->is_frozen() ? false : true;
		std::stringstream out;
		out << m_last_fired_dir << " " << nextdata.point.x << " " << nextdata.point.y << " " << m_current_damage;
		p->extradata = out.str();

		hit(static_cast<Player*>(userdata), p);
	
		m_current_damage = m_current_damage * m_penetrates_players;
		found_player = true;
	
		return p;
	}
}

float32 PenetrationGun::ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float32 fraction) {
	b2Body* body = fixture->GetBody();
	
	if (body->GetUserData() == NULL) {
		cerr << "Body has no user data!" << endl;
		return 1;
	}
	
	if (fraction < 0) {
		return 0;
	}
	
	HitData hit_data;
	hit_data.fixture = fixture;
	hit_data.point = point;
	hit_data.normal = normal;
	hit_data.fraction = fraction;
	
	m_hit_data.push_back(hit_data);
	
	return 1;
}
