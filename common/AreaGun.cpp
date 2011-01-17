/*
 * common/AreaGun.cpp
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

#include "AreaGun.hpp"
#include "common/Player.hpp"
#include "common/misc.hpp"
#include "common/MapObject.hpp"

using namespace LM;
using namespace std;

AreaGun::AreaGun(uint32_t id) : Weapon(id) {
	m_last_fired_time = 0;

	// TODO: We need a way to set this freeze time to the value from the client.
	m_freeze_time = 10000;
	
	m_freeze_on_hit = 0;
	m_damage = 100;
	m_cooldown = 700;
	m_recoil = 1.5;
	m_force = m_recoil;
	m_is_continuous = false;
	m_total_ammo = 0;
	m_ammo_recharge = 500;
	m_damage_degradation = 0;
	m_energy_cost = 0;
	
	m_area = NULL;

	m_physics = NULL;
	m_shot = NULL;
	m_firing_player = NULL;
	
	// TODO: What should we do about max_range?
	m_max_range = 10000;
}

bool AreaGun::parse_param(const char* param_string) {
	if (strncmp(param_string, "freeze=", 7) == 0) {
		m_freeze_on_hit = atol(param_string + 7);
	} else if (strncmp(param_string, "damage=", 7) == 0) {
		m_damage = atoi(param_string + 7);
	} else if (strncmp(param_string, "cooldown=", 9) == 0) {
		m_cooldown = atol(param_string + 9);
	} else if (strncmp(param_string, "recoil=", 7) == 0) {
		m_recoil = atof(param_string + 7);
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
	} else if (strncmp(param_string, "degradation=", 12) == 0) {
		m_damage_degradation = atof(param_string + 12);
	} else if (strncmp(param_string, "cost=", 5) == 0) {
		m_energy_cost = atoi(param_string + 5);
	} else if (strncmp(param_string, "shape=", 6) == 0) {
		m_area = make_shape_from_string(param_string + 6);
	} else if (strncmp(param_string, "force=", 6) == 0) {
		m_force = atof(param_string + 6);
	} else {
		return Weapon::parse_param(param_string);
	}
	return true;
}

Packet::WeaponDischarged* AreaGun::fire(b2World* physics, Player& player, Point start, float direction, Packet::WeaponDischarged* packet) {
	m_physics = physics;
	
	m_last_fired_time = get_ticks();
	
	// Recharge ammo, if applicable
	if (m_total_ammo) {
		m_current_ammo = get_current_ammo();
	
		if (m_current_ammo == 0) {
			// Out of ammo
			return NULL;
		}
		--m_current_ammo;
	}
	
	float currdirection = direction;
	
	b2BodyDef body_def;
	// TODO: Currently, all map objects are static; make this configured
	body_def.type = b2_staticBody;
	body_def.angle = currdirection;
	body_def.position.Set(to_physics(start.x), to_physics(start.y)); // 
	
	if (m_shot != NULL) {
		m_physics->DestroyBody(m_shot->get_physics_body());
		delete m_shot;
		m_shot = NULL;
	}
	
	m_shot = new Shot(this, &player);
	
	b2Body* newbody = physics->CreateBody(&body_def);
	m_shot->set_physics_body(newbody);
	m_shot->set_center(player.get_position());
	
	if (m_area == NULL) {
		WARN("No area for AreaGun that should have a bounding area.");
		return NULL;
	}
	
	b2FixtureDef areadef;

	areadef.shape = m_area;

	areadef.isSensor = true;
	
	newbody->CreateFixture(&areadef);
	
	newbody->SetUserData((void*)m_shot);
	
	packet->player_id = player.get_id();
	packet->weapon_id = get_id();
	std::stringstream out;
	out << start.x << " " << start.y << " " << direction;
	packet->extradata = out.str();
	
	was_fired(physics, player, out.str());
	
	return packet;
}

void AreaGun::was_fired(b2World* physics, Player& player, std::string extradata) {
	stringstream s(extradata);
	float start_x;
	float start_y;
	float direction;
	
	s >> start_x >> start_y >> direction;
	
	// Apply recoil and energy cost if necessary.
	player.apply_force(b2Vec2(m_recoil * 100 * cos(M_PI + direction), m_recoil * 100 * sin(M_PI + direction)));
	player.change_energy(-1 * m_energy_cost);
	if (player.get_energy() <= 0) {
		player.set_is_frozen(true, m_freeze_time);
	}
}

void AreaGun::hit(Player* hit_player, const Packet::PlayerHit* p) {
	string extradata = *p->extradata.item;
	stringstream s(extradata);
	
	float direction;
	float hit_point_x;
	float hit_point_y;
	float damage;
	
	s >> direction >> hit_point_x >> hit_point_y >> damage;
	
	float recoil = m_force;

	// Apply force to the player.
	hit_player->apply_force(b2Vec2(recoil * 100 * cos(direction), recoil * 100 * sin(direction)), b2Vec2(hit_point_x, hit_point_y));

	// Do damage if the gun has effect and they're not frozen.
	if (p->has_effect && !hit_player->is_frozen()) {
		// Apply damage to the player.
		hit_player->change_energy(-damage);
		
		// If player is damaged sufficiently, freeze them.
		if (hit_player->get_energy() <= 0) {
			hit_player->set_is_frozen(true, m_freeze_time);
		} else {
			// If we have an automatic freeze time, apply it:
			if (m_freeze_on_hit != 0) {
				hit_player->set_is_frozen(true, m_freeze_on_hit);
			}
		}	
	}
}

void AreaGun::hit_object(PhysicsObject* object, Shot* shot, b2Contact* contact) {
	if (object->get_type() == PhysicsObject::PLAYER) {
		Player* hit_player = static_cast<Player*>(object);
		
		// Ignore this if the player was the one who fired the shot.
		if (hit_player->get_id() == shot->get_firing_player()->get_id()) {
			return;
		}
		
		Point start = shot->get_center();
		
		// Check if we actually have a hit, by raycasting at the player's center and corners:
		m_hit_player_check = -1;
		
		m_shortest_dist = -1;
		m_physics->RayCast(this, b2Vec2(to_physics(start.x), to_physics(start.y)), b2Vec2(to_physics(hit_player->get_x()), to_physics(hit_player->get_y())));
		b2Body* body = hit_player->get_physics_body();
		// XXX: Do we just want to use the first fixture?
		b2Fixture* fixture = &body->GetFixtureList()[0];
		b2Shape* shape = fixture->GetShape();
		if (shape->GetType() == b2Shape::e_polygon) {
			b2PolygonShape* polyshape = static_cast<b2PolygonShape*>(shape);
			int index = 0;
			while (m_hit_player_check != hit_player->get_id() && index < polyshape->GetVertexCount()) {
				b2Vec2 vertex = polyshape->GetVertex(index);
			
				float x = to_physics(hit_player->get_x()) + vertex.x * cos(hit_player->get_rotation_radians());
				float y = to_physics(hit_player->get_y()) + vertex.y * sin(hit_player->get_rotation_radians());
				
				m_shortest_dist = -1;
				m_physics->RayCast(this, b2Vec2(to_physics(start.x), to_physics(start.y)), b2Vec2(x, y));
				if (m_hit_player_check == hit_player->get_id()) {
					break;
				}
				index++;
			}
		}
		
		if (m_hit_player_check != hit_player->get_id()) {
			return;
		}
		
		HitPlayer data;
		data.player = hit_player;
		data.angle = (hit_player->get_position() - start).get_angle();
		data.point = hit_player->get_position();
		m_hits.push_back(data);
	}
}

void AreaGun::reset() {
	m_last_fired_time = 0;
	m_current_ammo = m_total_ammo;
}

uint64_t AreaGun::get_remaining_cooldown() const {
	if (m_last_fired_time) {
		uint64_t	time_since_fire = get_ticks() - m_last_fired_time;
		if (time_since_fire < m_cooldown) {
			return m_cooldown - time_since_fire;
		}
	}
	return 0;
}

int AreaGun::get_current_ammo () const {
	if (m_last_fired_time && m_ammo_recharge) {
		// Take into account the ammo recharge that has occurred since last firing
		uint64_t	time_since_fire = get_ticks() - m_last_fired_time;
		return min<int>(m_total_ammo, m_current_ammo + time_since_fire / m_ammo_recharge);
	}
	return m_current_ammo;
}

Packet::PlayerHit* AreaGun::generate_next_hit_packet(Packet::PlayerHit* p, Player* shooter) {
	if (m_shot != NULL && m_physics != NULL && m_shot->get_physics_body() != NULL) {
		m_physics->DestroyBody(m_shot->get_physics_body());
		delete m_shot;
		m_shot = NULL;
	}
	
	if (m_hits.empty()) {
		return NULL;
	}
	
	HitPlayer nextdata = m_hits.back();
	Player* hit_player = nextdata.player;
	m_hits.pop_back();
	
	float actualdamage = m_damage - m_damage_degradation * (nextdata.point - hit_player->get_position()).get_magnitude();
	if (actualdamage <= 0) {
		actualdamage = 0;
	}

	p->shooter_id = shooter->get_id();
	p->weapon_id = get_id();

	p->shot_player_id = hit_player->get_id();
	p->has_effect = hit_player->is_frozen() ? false : true;
	std::stringstream out;
	
	out << nextdata.angle << " " << to_physics(nextdata.point.x) << " " << to_physics(nextdata.point.y) << " " << actualdamage;
	p->extradata = out.str();

	hit(hit_player, p);

	return p;
}

float32 AreaGun::ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float32 fraction) {
	b2Body* body = fixture->GetBody();
	
	if (body->GetUserData() == NULL) {
		WARN("Body has no user data!");
		return 1;
	}
	
	if (fraction < 0) {
		return 0;
	}
	
	if (fixture->IsSensor()) {
		return 1;
	}
	
	PhysicsObject* hitobj = static_cast<PhysicsObject*>(body->GetUserData());
	
	if (hitobj->get_type() == PhysicsObject::MAP_OBJECT) {
		MapObject* object = static_cast<MapObject*>(hitobj);
		if (!object->is_collidable()) {
			return 1;
		}
	}
	
	Point end = Point(point.x, point.y);
	Point start = Point(to_physics(m_shot->get_center().x), to_physics(m_shot->get_center().y));
	float dist = (end-start).get_magnitude();
	if (m_shortest_dist != -1 && dist > m_shortest_dist) {
		return 1;
	}
	m_shortest_dist = dist;
	
	if (hitobj->get_type() == PhysicsObject::PLAYER) {
		Player* hitplayer = static_cast<Player*>(hitobj);
		m_hit_player_check = hitplayer->get_id();
	} else {
		m_hit_player_check = -1;
	}
	
	return 1;
}
