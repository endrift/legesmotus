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
