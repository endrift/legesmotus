/*
 * client/GraphicalPlayer.hpp
 *
 * Copyright 2009 - Nathan Partlan, Andrew Ayer, Daniel Schneider, and Jeffrey Pfau
 * 
 */

#ifndef LM_CLIENT_GRAPHICALPLAYER_HPP
#define LM_CLIENT_GRAPHICALPLAYER_HPP

#include "common/Player.hpp"

class Sprite;

class GraphicalPlayer : public Player {
private:
	Sprite* m_sprite;
	double m_bounding_radius;
	

public:
	GraphicalPlayer();
	GraphicalPlayer(const char* name, uint32_t id, char team, double x, double y, double xvel, double yvel, double rotation);
	~GraphicalPlayer();

	Sprite* getSprite() const { return m_sprite; }
	double getRadius() const { return m_bounding_radius; }
	
	void setSprite(Sprite* s);
	void setRadius(double radius);
};

#endif
