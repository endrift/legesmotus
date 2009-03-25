/*
 * common/Player.cpp
 *
 * Copyright 2009 - Nathan Partlan, Andrew Ayer, Daniel Schneider, and Jeffrey Pfau
 * 
 */

#include "Player.hpp"

#include <cmath>
#include "common/math.hpp"

Player::Player() {
	m_id = 0;
	m_team = 0;
	m_x = 0;
	m_y = 0;
	m_x_vel = 0;
	m_y_vel = 0;
	m_rotation = 0;
	m_gun_rotation = 0;
	m_is_invisible = true;
	m_is_frozen = true;
}

Player::Player(const char* name, uint32_t id, char team, double x, double y, double xvel, double yvel, double rotation) : m_name(name) {
	m_id = id;
	m_team = team;
	m_x = x;
	m_y = y;
	m_x_vel = xvel;
	m_y_vel = yvel;
	m_rotation = rotation;
	m_gun_rotation = 0;
	m_is_invisible = true;
	m_is_frozen = true;
}

Player::~Player() {
}

double Player::get_rotation_radians() const {
	return m_rotation * DEGREES_TO_RADIANS;
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
	m_rotation = rotation;
}

void Player::set_rotation_radians(double rotation) {
	m_rotation = rotation * RADIANS_TO_DEGREES;
}

void Player::set_gun_rotation_degrees(double gun_rotation) {
	m_gun_rotation = gun_rotation;
}

void Player::set_gun_rotation_radians(double gun_rotation) {
	m_gun_rotation = gun_rotation * RADIANS_TO_DEGREES;
}

void Player::set_is_invisible(bool is_invisible) {
	m_is_invisible = is_invisible;
}

void Player::set_is_frozen(bool is_frozen) {
	m_is_frozen = is_frozen;
}


