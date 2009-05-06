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

GraphicGroup::GraphicGroup(const GraphicGroup& other) : Graphic(other) {
	std::map<Graphic*, Graphic*> old_ptr_map;
	for (list<Graphic*>::const_iterator iter = other.m_graphics.begin(); iter != other.m_graphics.end(); ++iter) {
		Graphic* g = (*iter)->clone();
		old_ptr_map[*iter] = g;
		m_graphics.push_back(g); // Keep track of the mapping between the old pointer and the new pointer
	}
	for (map<string, Graphic*>::const_iterator iter = other.m_names.begin(); iter != other.m_names.end(); ++iter) {
		m_names[iter->first] = old_ptr_map[iter->second]; // Update the names to point to the new graphic
	}
}

GraphicGroup::~GraphicGroup() {
	for (list<Graphic*>::iterator iter = m_graphics.begin(); iter != m_graphics.end(); ++iter) {
		delete *iter;
	}
}

GraphicGroup* GraphicGroup::clone() const {
	return new GraphicGroup(*this);
}

void GraphicGroup::add_graphic(Graphic* graphic) {
	for (list<Graphic*>::iterator iter = m_graphics.begin(); iter != m_graphics.end(); ++iter) {
		if ((*iter)->get_priority() < graphic->get_priority()) {
			m_graphics.insert(iter,graphic->clone());
			return;
		}
	}
	m_graphics.push_back(graphic->clone());
}

void GraphicGroup::add_graphic(Graphic* graphic, const string& name) {
	Graphic* g = graphic->clone();
	for (list<Graphic*>::iterator iter = m_graphics.begin(); iter != m_graphics.end(); ++iter) {
		if ((*iter)->get_priority() < graphic->get_priority()) {
			m_graphics.insert(iter,g);
			m_names[name] = g;
			return;
		}
	}
	m_graphics.push_back(g);
	m_names[name] = g;
}

Graphic* GraphicGroup::get_graphic(const string& name) {
	return m_names[name];
}

void GraphicGroup::remove_graphic(const string& name) {
	m_graphics.remove(get_graphic(name));
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
	for (list<Graphic*>::const_iterator iter = m_graphics.begin(); iter != m_graphics.end(); ++iter) {
		if (!(*iter)->is_invisible()) {
			(*iter)->draw(window);
		}
	}
	glPopMatrix();
}
