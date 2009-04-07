/*
 * client/MapObject.cpp
 *
 * Copyright 2009 - Nathan Partlan, Andrew Ayer, Daniel Schneider, and Jeffrey Pfau
 * 
 */

#include "MapObject.hpp"

MapObject::MapObject() {
	m_type = 0;
}
MapObject::MapObject(int type, Sprite* sprite, Point upper_left) : m_sprite(sprite) {
	m_type = type;
	m_upper_left = upper_left;
}

