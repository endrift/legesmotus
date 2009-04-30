/*
 * client/GraphicalPlayer.hpp
 *
 * Copyright 2009 - Nathan Partlan, Andrew Ayer, Daniel Schneider, and Jeffrey Pfau
 * 
 */

#ifndef LM_CLIENT_GRAPHICALPLAYER_HPP
#define LM_CLIENT_GRAPHICALPLAYER_HPP

#include "common/Player.hpp"
#include "GraphicGroup.hpp"

class Sprite;
class Graphic;

class GraphicalPlayer : public Player {
private:
	GraphicGroup* 		m_sprite;
	Graphic*		m_name_sprite;
	double 			m_bounding_radius;

public:
	GraphicalPlayer();
	GraphicalPlayer(const char* name, uint32_t id, char team, GraphicGroup* sprite = NULL, double x = 0, double y = 0, double xvel = 0, double yvel = 0, double rotation = 0);
	~GraphicalPlayer();

	GraphicGroup* get_sprite() const { return m_sprite; }
	Graphic* get_name_sprite() const { return m_name_sprite; }
	double get_radius() const { return m_bounding_radius; }
	//double get_width();
	//double get_height();
		
	void set_sprite(GraphicGroup* s);
	void set_name_sprite(Graphic* ns);
	void set_radius(double radius);
	void set_is_invisible(bool is_invisible);
	void set_x(double x);
	void set_y(double y);
	void set_rotation_degrees(double rotation);
	void set_rotation_radians(double rotation);
	void set_is_frozen(bool is_frozen);
};

#endif
