/*
 * client/Minimap.cpp
 *
 * Copyright 2009 - Nathan Partlan, Andrew Ayer, Daniel Schneider, and Jeffrey Pfau
 * 
 */

#include "Minimap.hpp"
#include "GameWindow.hpp"
#include <string>
#include <sstream>

using namespace std;

Minimap::Minimap(double scale) {
	m_whole = new GraphicGroup;

	Sprite mask_sprite("data/sprites/mini_circle.png");
	GraphicGroup group;
	Mask mask(&mask_sprite,&group);
	Sprite background("data/sprites/mini_bg.png");

	mask.set_priority(Graphic::FOREGROUND);
	background.set_priority(Graphic::BACKGROUND);
	m_whole->add_graphic(&mask,"mask");
	m_whole->add_graphic(&background);

	m_minimask = static_cast<Mask*>(m_whole->get_graphic("mask"));
	m_minigroup = static_cast<GraphicGroup*>(m_minimask->get_masked());

	m_master_blip_red = new Sprite("data/sprites/mini_blip_red.png");
	m_master_blip_blue = new Sprite("data/sprites/mini_blip_blue.png");

	m_scale = scale;
}

Minimap::~Minimap() {
	delete m_whole;
	delete m_master_blip_red;
	delete m_master_blip_blue;
}

void Minimap::set_x(double x) {
	m_whole->set_x(x);
}

void Minimap::set_y(double y) {
	m_whole->set_y(y);
}

void Minimap::set_invisible(bool enable) {
	m_whole->set_invisible(enable);
}

void Minimap::add_blip(uint32_t id, char team, double x, double y) {
	Sprite *clone;
	if (team == 'A') {
		clone = m_master_blip_blue->clone();
	} else {
		clone = m_master_blip_red->clone();
	}
	clone->set_x(x*m_scale);
	clone->set_y(y*m_scale);
	stringstream s;
	s << id; // Why is STL so bad?
	m_minigroup->add_graphic(clone,s.str());
	delete clone;
}

void Minimap::move_blip(uint32_t id, double x, double y) {
	Graphic *blip;
	stringstream s;
	s << id;
	blip = m_minigroup->get_graphic(s.str());
	if (blip != NULL) {
		blip->set_x(x*m_scale);
		blip->set_y(y*m_scale);
	}
}

void Minimap::remove_blip(uint32_t id) {
	Graphic *blip;
	stringstream s;
	s << id;
	blip = m_minigroup->get_graphic(s.str());
	m_minigroup->remove_graphic(s.str());
	delete blip;
}

void Minimap::recenter(double x, double y) {
	m_minigroup->set_center_x(x*m_scale);
	m_minigroup->set_center_y(y*m_scale);
}

void Minimap::register_with_window(GameWindow* window) {
	window->register_hud_graphic(m_whole);
}

void Minimap::unregister_with_window(GameWindow* window) {
	window->unregister_hud_graphic(m_whole);
}
