/*
 * common/Player.cpp
 *
 * This file is part of Leges Motus, a networked, 2D shooter set in zero gravity.
 * 
 * Copyright 2009 Andrew Ayer, Nathan Partlan, Jeffrey Pfau
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
#include "common/PacketReader.hpp"
#include "common/PacketWriter.hpp"

using namespace LM;
using namespace std;

// See .hpp file for extensive comments.

Player::Player() {
	m_id = 0;
	m_team = 0;
	m_score = 0;
	m_health = MAX_HEALTH;
	m_x = 0;
	m_y = 0;
	m_x_vel = 0;
	m_y_vel = 0;
	m_rotation = 0;
	m_rotational_vel = 0;
	m_gun_rotation = 0;
	m_is_invisible = true;
	m_is_frozen = true;
}

Player::Player(const char* name, uint32_t id, char team, double x, double y, double xvel, double yvel, double rotation) : m_name(name) {
	m_id = id;
	m_team = team;
	m_score = 0;
	m_health = MAX_HEALTH;
	m_x = x;
	m_y = y;
	m_x_vel = xvel;
	m_y_vel = yvel;
	m_rotation = rotation;
	m_rotational_vel = 0;
	m_gun_rotation = 0;
	m_is_invisible = true;
	m_is_frozen = true;
}

Player::~Player() {
}

double Player::get_rotation_radians() const {
	return m_rotation * DEGREES_TO_RADIANS;
}

double Player::get_rotational_vel_radians() const {
	return m_rotational_vel * DEGREES_TO_RADIANS;
}

double Player::get_gun_rotation_radians() const {
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

void Player::set_health(int health) {
	m_health = health;
	if (m_health > MAX_HEALTH) {
		m_health = MAX_HEALTH;
	} else if (m_health < 0) {
		m_health = 0;
	}
}

void Player::change_health(int health_change) {
	set_health(m_health + health_change);
}

void Player::set_x(double x) {
	m_x = x;
}

void Player::set_y(double y) {
	m_y = y;
}

void Player::set_position(double x, double y) {
	set_x(x);
	set_y(y);
}

void Player::update_position(unsigned long timediff) {
	set_x(m_x + m_x_vel * timediff);
	set_y(m_y + m_y_vel * timediff);
	set_rotation_degrees(m_rotation + m_rotational_vel * timediff);
}

void Player::set_x_vel(double xvel) {
	m_x_vel = xvel;
}

void Player::set_y_vel(double yvel) {
	m_y_vel = yvel;
}

void Player::set_velocity(double xvel, double yvel) {
	set_x_vel(xvel);
	set_y_vel(yvel);
}

void Player::set_rotation_degrees(double rotation) {
	m_rotation = get_normalized_angle(rotation);
}

void Player::set_rotation_radians(double rotation) {
	m_rotation = get_normalized_angle(rotation * RADIANS_TO_DEGREES);
}

void Player::set_rotational_vel(double rotation) {
	m_rotational_vel = rotation;
}

void Player::set_rotational_vel_radians(double rotation) {
	m_rotational_vel = rotation * RADIANS_TO_DEGREES;
}

void Player::set_gun_rotation_degrees(double gun_rotation) {
	m_gun_rotation = get_normalized_angle(gun_rotation);
}

void Player::set_gun_rotation_radians(double gun_rotation) {
	m_gun_rotation = get_normalized_angle(gun_rotation * RADIANS_TO_DEGREES);
}

void Player::set_is_invisible(bool is_invisible) {
	m_is_invisible = is_invisible;
}

void Player::set_is_frozen(bool is_frozen) {
	m_is_frozen = is_frozen;
}


void Player::write_update_packet (PacketWriter& packet) const {
	string	flags;
	
	if (is_invisible())	{ flags.push_back('I'); }
	if (is_frozen())	{ flags.push_back('F'); }

	packet << get_id() << get_x() << get_y() << get_x_vel() << get_y_vel() << get_rotation_degrees() << flags;
}

void Player::read_update_packet (PacketReader& packet) {
	double	x;
	double	y;
	double	x_vel;
	double	y_vel;
	double	rotation;
	string	flags;

	packet >> x >> y >> x_vel >> y_vel >> rotation >> flags;

	set_position(x, y);
	set_velocity(x_vel, y_vel);
	set_rotation_degrees(rotation);
	set_is_invisible(flags.find_first_of('I') != string::npos);
	set_is_frozen(flags.find_first_of('F') != string::npos);
}

