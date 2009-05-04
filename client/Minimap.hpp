/*
 * client/Minimap.hpp
 *
 * Copyright 2009 - Nathan Partlan, Andrew Ayer, Daniel Schneider, and Jeffrey Pfau
 * 
 */

#ifndef LM_CLIENT_MINIMAP_HPP
#define LM_CLIENT_MINIMAP_HPP

#include "Mask.hpp"
#include "Sprite.hpp"
#include "GraphicGroup.hpp"
#include <stdint.h>

class Minimap {
private:
	Mask*		m_minimask;
	GraphicGroup*	m_minigroup;
	GraphicGroup*	m_whole;
	Sprite*		m_master_blip_red;
	Sprite*		m_master_blip_blue;
	double		m_scale;
public:
	Minimap(double scale = 0.125); // TODO pass PathManager
	~Minimap();

	void set_x(double x);
	void set_y(double y);
	void set_invisible(bool enable);

	void add_blip(uint32_t id, char team, double x, double y);
	void move_blip(uint32_t id, double x, double y);
	// TODO set_blip_invisible
	void remove_blip(uint32_t id);

	void recenter(double x, double y);

	void register_with_window(GameWindow* window);
	void unregister_with_window(GameWindow* window);
};

#endif
