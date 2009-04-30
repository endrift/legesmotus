/*
 * client/GraphicGroup.cpp
 *
 * Copyright 2009 - Nathan Partlan, Andrew Ayer, Daniel Schneider, and Jeffrey Pfau
 * 
 */

#include "GraphicGroup.hpp"

using namespace std;

GraphicGroup::GraphicGroup() : Graphic() {
	// Nothing to do
}

void GraphicGroup::add_graphic(Graphic* graphic) {
	for (list<Graphic*>::iterator iter = m_graphics.begin(); iter != m_graphics.end(); ++iter) {
		if ((*iter)->get_priority() < graphic->get_priority()) {
			m_graphics.insert(iter,graphic);
			return;
		}
	}
	m_graphics.push_back(graphic);
}

void GraphicGroup::remove_graphic(Graphic* graphic) {
	m_graphics.remove(graphic);
}

void GraphicGroup::set_alpha(double alpha) {
	for (list<Graphic*>::iterator iter = m_graphics.begin(); iter != m_graphics.end(); ++iter) {
		(*iter)->set_alpha(alpha);
	}
}

void GraphicGroup::set_red_intensity(double r) {
	for (list<Graphic*>::iterator iter = m_graphics.begin(); iter != m_graphics.end(); ++iter) {
		(*iter)->set_red_intensity(r);
	}
}

void GraphicGroup::set_green_intensity(double g) {
	for (list<Graphic*>::iterator iter = m_graphics.begin(); iter != m_graphics.end(); ++iter) {
		(*iter)->set_green_intensity(g);
	}
}

void GraphicGroup::set_blue_intensity(double b) {
	for (list<Graphic*>::iterator iter = m_graphics.begin(); iter != m_graphics.end(); ++iter) {
		(*iter)->set_blue_intensity(b);
	}
}

void GraphicGroup::draw(const GameWindow* window) const {
	glPushMatrix();
	transform_gl();
	for (std::list<Graphic*>::const_iterator iter = m_graphics.begin(); iter != m_graphics.end(); ++iter) {
		if (!(*iter)->is_invisible()) {
			(*iter)->draw(window);
		}
	}
	glPopMatrix();
}
