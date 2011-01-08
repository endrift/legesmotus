/*
 * common/Player.cpp
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

#include "Player.hpp"

#include <string>
#include <cmath>
#include "common/math.hpp"
#include "common/Packet.hpp"
#include "common/PacketReader.hpp"
#include "common/PacketWriter.hpp"
#include "common/timer.hpp"
#include "common/physics.hpp"
#include <iostream>
#include "common/misc.hpp"

using namespace LM;
using namespace std;

// See .hpp file for extensive comments.

Player::Player(b2World* physics_world) {
	m_physics_body = NULL;
	m_id = 0;
	m_team = 0;
	m_score = 0;
	m_energy = MAX_ENERGY;
	m_x = 0;
	m_y = 0;
	m_x_vel = 0;
	m_y_vel = 0;
	m_rotation = 0;
	m_rotational_vel = 0;
	m_gun_rotation = 0;
	m_freeze_time = -1;
	m_is_invisible = true;
	m_is_frozen = true;
	m_is_grabbing_obstacle = false;
	m_attach_joint = NULL;
	m_physics = NULL;
	m_current_weapon_id = 0;
	
	if (physics_world != NULL) {
		m_physics = physics_world;
		initialize_physics(physics_world);
	}
}

Player::Player(const char* name, uint32_t id, char team, float x, float y, float xvel, float yvel, float rotation, b2World* physics_world) : m_name(name) {
	m_physics_body = NULL;
	m_id = id;
	m_team = team;
	m_score = 0;
	m_energy = MAX_ENERGY;
	m_x = x;
	m_y = y;
	m_x_vel = xvel;
	m_y_vel = yvel;
	m_rotation = rotation;
	m_rotational_vel = 0;
	m_gun_rotation = 0;
	m_freeze_time = -1;
	m_is_invisible = true;
	m_is_frozen = true;
	m_is_grabbing_obstacle = false;
	m_attach_joint = NULL;
	m_physics = NULL;
	m_current_weapon_id = 0;
	
	if (physics_world != NULL) {
		m_physics = physics_world;
		initialize_physics(physics_world);
	}
}

Player::~Player() {
	if (m_physics != NULL) {
		if (m_attach_joint != NULL) {
			m_physics->DestroyJoint(m_attach_joint);
			m_attach_joint = NULL;
		}
		m_physics->DestroyBody(m_physics_body);
	}
}

void Player::initialize_physics(b2World* world) {
	if (world != NULL) {
		m_physics = world;
	}

	// TODO: TESTING CODE for physics - might want to specify
	// this initialization in a config file or something
	b2BodyDef bodydef;
	bodydef.type = b2_dynamicBody;
	bodydef.position.Set(to_physics(m_x), to_physics(m_y));
	bodydef.allowSleep = false;
	m_physics_body = world->CreateBody(&bodydef);
	m_physics_body->SetUserData((void*)this);
	
	b2PolygonShape playerbox;
	playerbox.SetAsBox(to_physics(18.0f), to_physics(40.0f));

	b2FixtureDef fixturedef;
	fixturedef.shape = &playerbox;
	fixturedef.density = 1.0f;
	fixturedef.friction = 0.0f;
	fixturedef.restitution = 0.8f;

	m_physics_body->CreateFixture(&fixturedef);
}

float Player::get_rotation_radians() const {
	return m_rotation * DEGREES_TO_RADIANS;
}

float Player::get_rotational_vel_radians() const {
	return m_rotational_vel * DEGREES_TO_RADIANS;
}

float Player::get_gun_rotation_radians() const {
	return m_gun_rotation * DEGREES_TO_RADIANS;
}

void Player::set_name(const char* name) {
	m_name = name;
}

void Player::set_id(uint32_t id) {
	m_id = id;
}

void Player::set_team(char team) {
	m_team = team;
}

void Player::set_score(int score) {
	m_score = score;
}

void Player::add_score(int score_increase) {
	m_score += score_increase;
}

void Player::set_energy(int energy) {
	m_energy = energy;
	if (m_energy > MAX_ENERGY) {
		m_energy = MAX_ENERGY;
	} else if (m_energy < 0) {
		m_energy = 0;
	}
}

void Player::change_energy(int energy_change) {
	set_energy(m_energy + energy_change);
}

void Player::set_x(float x) {
	set_position(x, m_y);
}

void Player::set_y(float y) {
	set_position(m_x, y);
}

void Player::set_position(float x, float y) {
	if (m_physics_body != NULL) {
		m_physics_body->SetTransform(b2Vec2(to_physics(x), to_physics(y)), get_rotation_radians());
	}
	m_x = x;
	m_y = y;
	update_location();
}

void Player::apply_force(b2Vec2 force_vec) {
	m_physics_body->ApplyForce(force_vec, m_physics_body->GetWorldCenter());
}

void Player::apply_force(b2Vec2 force_vec, b2Vec2 world_point) {
	m_physics_body->ApplyForce(force_vec, world_point);
}

void Player::apply_torque(float torque) {
 	m_physics_body->ApplyTorque(torque);
}

void Player::update_physics() {
	if (m_physics_body != NULL) {
		m_x = to_game(m_physics_body->GetPosition().x);
		m_y = to_game(m_physics_body->GetPosition().y);
		m_rotation = to_degrees(m_physics_body->GetAngle());

		update_location();

		// Prevent rotational velocity from going too high.
		if (m_physics_body->GetAngularVelocity() > MAX_ANGULAR_VELOCITY) {
			m_physics_body->SetAngularVelocity(MAX_ANGULAR_VELOCITY);
		} else if (m_physics_body->GetAngularVelocity() < -1.0 * MAX_ANGULAR_VELOCITY) {
			m_physics_body->SetAngularVelocity(-1.0 * MAX_ANGULAR_VELOCITY);
		}
	}
	
	if (m_is_frozen) {
		if (m_freeze_time >= 0 && m_freeze_time < (long)(get_ticks() - m_frozen_at)) {
			set_is_frozen(false);
			set_energy(MAX_ENERGY);
		}
	}
}

void Player::update_position(float timescale) {
	set_position(m_x + m_x_vel * timescale, m_y + m_y_vel * timescale);
}

void Player::update_rotation(float timescale) {
	set_rotation_degrees(m_rotation + m_rotational_vel * timescale);
}

void Player::stop() {
	set_velocity(0, 0);
	set_rotational_vel(0);
}

void Player::bounce(float angle_of_incidence, float velocity_scale) {
	float		curr_magnitude = get_velocity().get_magnitude();
	float		curr_angle = to_degrees(get_velocity().get_angle());
	float		new_angle = get_normalized_angle(angle_of_incidence + (angle_of_incidence - curr_angle) - 180);

	set_velocity(Vector::make_from_magnitude(curr_magnitude * velocity_scale, to_radians(new_angle)));
}

void Player::set_velocity(Vector vel) {
	m_x_vel = vel.x;
	m_y_vel = vel.y;
	if (m_physics_body != NULL) {
		m_physics_body->SetLinearVelocity(b2Vec2(to_physics(vel.x), to_physics(vel.y)));
	}
	if (is_moving()) {
		// Moving players cannot be grabbing obstacles
		set_is_grabbing_obstacle(false);
	}
}

void Player::set_rotation_degrees(float rotation) {
	m_rotation = get_normalized_angle(rotation);
	if (m_physics_body != NULL) {
		m_physics_body->SetTransform(b2Vec2(to_physics(m_x), to_physics(m_y)), to_radians(m_rotation));
	}
}

void Player::set_rotation_radians(float rotation) {
	set_rotation_degrees(to_degrees(rotation));
}

void Player::set_rotational_vel(float rotation) {
	m_rotational_vel = rotation;
	update_location();
}

void Player::set_rotational_vel_radians(float rotation) {
	set_rotational_vel(rotation * RADIANS_TO_DEGREES);
}

void Player::set_gun_rotation_degrees(float gun_rotation) {
	m_gun_rotation = get_normalized_angle(gun_rotation);
}

void Player::set_gun_rotation_radians(float gun_rotation) {
	set_gun_rotation_degrees(gun_rotation * RADIANS_TO_DEGREES);
}

void Player::set_is_invisible(bool is_invisible) {
	m_is_invisible = is_invisible;
}

void Player::set_is_frozen(bool is_frozen) {
	m_is_frozen = is_frozen;
	
	if (is_frozen) {
		set_is_grabbing_obstacle(false);
	}
}

void Player::set_is_frozen(bool is_frozen, long freeze_time) {
	m_is_frozen = is_frozen;
	
	if (is_frozen) {
		m_freeze_time = freeze_time;
		m_frozen_at = get_ticks();
		set_is_grabbing_obstacle(false);
	}
}

void Player::set_freeze_time(long freeze_time) {
	m_freeze_time = freeze_time;
}

void Player::set_current_weapon_id(long current_weapon_id) {
	m_current_weapon_id = current_weapon_id;
}

void Player::set_attach_joint(b2Joint* joint) {
	if (m_attach_joint != NULL && m_physics != NULL) {
		m_physics->DestroyJoint(m_attach_joint);
	}

	m_attach_joint = joint;
	
	m_is_grabbing_obstacle = (m_attach_joint != NULL);
}

void Player::write_update_packet (PacketWriter& packet) const {
	string	flags;
	
	if (is_invisible())		{ flags.push_back('I'); }
	if (is_frozen())		{ flags.push_back('F'); }
	if (is_grabbing_obstacle())	{ flags.push_back('G'); }

	packet << get_id() << get_x() << get_y() << get_x_vel() << get_y_vel() << get_rotation_degrees() << get_energy() << get_gun_rotation_degrees() << get_current_weapon_id() << flags;
}

void Player::read_update_packet (PacketReader& packet) {
	float x;
	float y;
	float x_vel;
	float y_vel;
	float rotation;
	float aim;
	int energy;
	uint32_t current_weapon_id;
	string	flags;

	packet >> x >> y >> x_vel >> y_vel >> rotation >> energy >> aim >> current_weapon_id >> flags;

	// Note: We must use the setter functions, and not set the values directly, since a derived class may have overridden a setter.  (GraphicalPlayer overrides nearly all of them.)
	set_position(x, y);
	set_velocity(x_vel, y_vel);
	set_rotation_degrees(rotation);
	set_energy(energy);
	set_gun_rotation_degrees(aim);
	set_current_weapon_id(current_weapon_id);
	set_is_invisible(flags.find_first_of('I') != string::npos);
	set_is_frozen(flags.find_first_of('F') != string::npos);
	set_is_grabbing_obstacle(flags.find_first_of('G') != string::npos);
}

void Player::set_is_grabbing_obstacle(bool x) {
	m_is_grabbing_obstacle = x;
}

void Player::generate_player_update(Packet::PlayerUpdate* p) {
	p->player_id = get_id();
	p->x = get_x();
	p->y = get_y();
	p->x_vel = get_x_vel();
	p->y_vel = get_y_vel();
	p->rotation = get_rotation_degrees();
	p->energy = get_energy();
	p->gun_rotation = get_gun_rotation_degrees();
	p->current_weapon_id = get_current_weapon_id();
	p->flags = "";
	if (is_invisible()) {
		p->flags->append(1, 'I');
	}
	if (is_frozen()) {
		p->flags->append(1, 'F');
	}
	if (is_grabbing_obstacle()) {
		p->flags->append(1, 'G');
	}
}

void Player::read_player_update(const Packet::PlayerUpdate& p) {
	set_position(p.x, p.y);
	set_velocity(p.x_vel, p.y_vel);
	set_rotation_degrees(p.rotation);
	set_energy(p.energy);
	set_gun_rotation_degrees(p.gun_rotation);
	set_current_weapon_id(p.current_weapon_id);
	//set_is_invisible(p.flags->find_first_of('I') != string::npos);
	set_is_frozen(p.flags->find_first_of('F') != string::npos);
	set_is_grabbing_obstacle(p.flags->find_first_of('G') != string::npos);
	set_is_invisible(p.flags->find_first_of('I') != string::npos);
	if (is_grabbing_obstacle() && m_physics_body != NULL) {
		// Let the other client say how we're moving on the wall
		m_physics_body->SetAwake(false);
	}
}
