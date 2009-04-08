/*
 * client/GraphicalPlayer.cpp
 *
 * Copyright 2009 - Nathan Partlan, Andrew Ayer, Daniel Schneider, and Jeffrey Pfau
 * 
 */

#include "GraphicalPlayer.hpp"
#include "Sprite.hpp"
#include "common/math.hpp"

#include <iostream>

GraphicalPlayer::GraphicalPlayer() : Player::Player() {
	m_sprite = NULL;
}

GraphicalPlayer::GraphicalPlayer(const char* name, uint32_t id, char team, Sprite* sprite, double x, double y, double xvel, double yvel, double rotation)
: Player::Player(name, id, team, x, y, xvel, yvel, rotation) {
	m_sprite = sprite;
	if (m_sprite != NULL) {
		m_sprite->set_x(x);
		m_sprite->set_y(y);
		m_sprite->set_rotation(rotation);
	}
}

GraphicalPlayer::~GraphicalPlayer() {
	
}

double GraphicalPlayer::get_width() {
	if (m_sprite != NULL) {
		return m_sprite->get_width();
	} else {
		return 0;
	}
}

double GraphicalPlayer::get_height() {
	if (m_sprite != NULL) {
		return m_sprite->get_height();
	} else {
		return 0;
	}
}

void GraphicalPlayer::set_sprite(Sprite* s) {
	m_sprite = s;
	if (m_sprite != NULL) {
		m_sprite->set_x(m_x);
		m_sprite->set_y(m_y);
		m_sprite->set_rotation(m_rotation);
	}
}

void GraphicalPlayer::set_radius(double radius) {
	if (radius >= 0) {
		m_bounding_radius = radius;
	} else {
		cerr << "Error: Radius of player cannot be less than 0. Was: " << radius << endl;
	}
}

void GraphicalPlayer::set_is_invisible(bool is_invisible) {
	m_is_invisible = is_invisible;
	if (m_sprite != NULL) {
		if (is_invisible) {
			m_sprite->set_alpha(255);
		} else {
			m_sprite->set_alpha(0);
		}
	}
}

void GraphicalPlayer::set_x(double x) {
	m_x = x;
	if (m_sprite != NULL) {
		m_sprite->set_x(x);
	}
}

void GraphicalPlayer::set_y(double y) {
	m_y = y;
	if (m_sprite != NULL) {
		m_sprite->set_y(y);
	}
}

void GraphicalPlayer::set_rotation_degrees(double rotation) {
	m_rotation = rotation;
	if (m_sprite != NULL) {
		m_sprite->set_rotation(m_rotation);
	}
}

void GraphicalPlayer::set_rotation_radians(double rotation) {
	m_rotation = rotation * RADIANS_TO_DEGREES;
	if (m_sprite != NULL) {
		m_sprite->set_rotation(m_rotation);
	}
}