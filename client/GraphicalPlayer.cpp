/*
 * client/GraphicalPlayer.cpp
 *
 * Copyright 2009 - Nathan Partlan, Andrew Ayer, Daniel Schneider, and Jeffrey Pfau
 * 
 */

#include "GraphicalPlayer.hpp"
#include "Sprite.hpp"

#include <iostream>

GraphicalPlayer::GraphicalPlayer() : Player::Player() {
}

GraphicalPlayer::GraphicalPlayer(const char* name, uint32_t id, char team, double x, double y, double xvel, double yvel, double rotation)
: Player::Player(name, id, team, x, y, xvel, yvel, rotation) {

}

GraphicalPlayer::~GraphicalPlayer() {
	
}


void GraphicalPlayer::setSprite(Sprite* s) {
	m_sprite = s;
}

void GraphicalPlayer::setRadius(double radius) {
	if (radius >= 0) {
		m_bounding_radius = radius;
	} else {
		cerr << "Error: Radius of player cannot be less than 0. Was: " << radius << endl;
	}
}
