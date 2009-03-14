/*
 * common/Player.cpp
 *
 * Copyright 2009 - Nathan Partlan, Andrew Ayer, Daniel Schneider, and Jeffrey Pfau
 * 
 */

#include "Player.hpp"

#include <cmath>

Player::Player() {
	m_x = 0;
	m_y = 0;
	m_x_vel = 0;
	m_y_vel = 0;
	m_rotation = 0;
}

Player::Player(double x, double y, double xvel, double yvel, double rotation) {
	m_x = x;
	m_y = y;
	m_x_vel = xvel;
	m_y_vel = yvel;
	m_rotation = rotation;
}

Player::~Player() {
}

double Player::PI() {
	return (4.0*atan(1.0));
}

double Player::get_x() {
	return m_x;
}

double Player::get_y() {
	return m_y;
}

double Player::get_x_vel() {
	return m_x_vel;
}

double Player::get_y_vel() {
	return m_y_vel;
}

double Player::get_rotation_degrees() {
	return m_rotation;
}

double Player::get_rotation_radians() {
	return m_rotation * PI() / 180;
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
	m_rotation = rotation * PI() / 180;
}


