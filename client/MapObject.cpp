/*
 * client/MapObject.cpp
 *
 * Copyright 2009 - Nathan Partlan, Andrew Ayer, Daniel Schneider, and Jeffrey Pfau
 * 
 */

#include "MapObject.hpp"

MapObject::MapObject(int type, Point upper_left) {
	m_type = type;
	m_upper_left = upper_left;
	m_sprite = NULL;
	m_team = 0;
}

void MapObject::set_sprite(Graphic* s) {
	m_sprite = s;
	if (m_sprite != NULL) {
		m_sprite->set_x(m_upper_left.x);
		m_sprite->set_y(m_upper_left.y);
	}
}

void MapObject::set_sprite(Sprite* s) {
	// Make the sprite draw from the upper left, not the center
	s->set_center_x(0.0);
	s->set_center_y(0.0);

	Graphic* g = s;
	set_sprite(g);
}

