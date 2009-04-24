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

GraphicGroup::~GraphicGroup() {
	m_graphics.clear();
}

void GraphicGroup::add_graphic(Graphic* graphic) {
	for (list<Graphic*>::iterator iter = m_graphics.begin(); iter != m_graphics.end(); ++iter) {
		if ((*iter)->get_priority() < graphic->get_priority()) {
			m_graphics.insert(iter,graphic);
			return;
		}
	}
	m_graphics.push_back(graphic);
	if (graphic->get_image_width() > m_image_width) {
		m_image_width = graphic->get_image_width();
	}
	if (graphic->get_image_height() > m_image_height) {
		m_image_height = graphic->get_image_height();
	}
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

int GraphicGroup::get_image_width() const {
	return m_image_width;
}

int GraphicGroup::get_image_height() const {
	return m_image_height;
}
