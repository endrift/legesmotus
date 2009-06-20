/*
 * client/GraphicGroup.cpp
 *
 * This file is part of Leges Motus, a networked, 2D shooter set in zero gravity.
 * 
 * Copyright 2009 Andrew Ayer, Nathan Partlan, Jeffrey Pfau
 * 
 * Leges Motus is free and open source software.  You may redistribute it and/or
 * modify it under the terms of version 2, or (at your option) version 3, of the
 * GNU General Public License (GPL), as published by the Free Software Foundation.
 * 
 * Leges Motus is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
 * PARTICULAR PURPOSE.  See the full text of the GNU General Public License for
 * further detail.
 * 
 * For a full copy of the GNU General Public License, please see the COPYING file
 * in the root of the source code tree.  You may also retrieve a copy from
 * <http://www.gnu.org/licenses/gpl-2.0.txt>, or request a copy by writing to the
 * Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 * 02111-1307  USA
 * 
 */

#include "GraphicGroup.hpp"

using namespace LM;
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
	if (m_names.find(name) != m_names.end()) {
		return m_names[name];
	} else {
		return NULL;
	}
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
