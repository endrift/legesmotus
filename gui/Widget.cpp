/*
 * gui/Widget.cpp
 *
 * This file is part of Leges Motus, a networked, 2D shooter set in zero gravity.
 * 
 * Copyright 2009-2010 Andrew Ayer, Nathan Partlan, Jeffrey Pfau
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

#include "Widget.hpp"
#include <boost/bind.hpp>

using namespace LM;
using namespace std;
using namespace boost;

Widget::Widget(Widget* parent) {
	m_parent = parent;
	if (parent != NULL) {
		parent->add_child(this);
	}

	m_x = 0;
	m_y = 0;
	m_w = 0;
	m_h = 0;

	m_drawable = true;
}

Widget::~Widget() {
	set_parent(NULL);
	clear_children();
}

void Widget::set_parent(Widget* new_parent) {
	if (new_parent == m_parent) {
		return;
	}

	if (m_parent != NULL) {
		m_parent->remove_child(this);
	}

	if (new_parent != NULL) {
		new_parent->add_child(this);
	}

	m_parent = new_parent;
}

Widget* Widget::get_parent() {
	return m_parent;
}

void Widget::add_child(Widget* child, int priority) {
	if (child != NULL) {
		if (child->m_parent != NULL) {
			child->m_parent->remove_child(child);
		}
		child->m_parent = this;
		m_children.insert(make_pair(priority, child));
	}
}

void Widget::remove_child(Widget* child) {
	if (child != NULL) {
		child->m_parent = NULL;
		for (multimap<int, Widget*>::iterator iter = m_children.begin(); iter != m_children.end(); ++iter) {
			if (iter->second == child) {
				// Return the iterator to delete, but decrement iterator so it's not invalidated
				m_children.erase(iter--);
			}
		}
	}
}

void Widget::clear_children() {
	for (multimap<int, Widget*>::iterator iter = m_children.begin(); iter != m_children.end(); ++iter) {
		iter->second->m_parent = NULL;
		delete iter->second;
	}
}

const multimap<int, Widget*>& Widget::get_children() {
	return m_children;
}

Widget* Widget::top_child_at(float x, float y) {
	for (multimap<int, Widget*>::iterator iter = m_children.begin(); iter != m_children.end(); ++iter) {
		if (iter->second->contains_point(x - get_x(), y - get_y())) {
			return iter->second;
		}
	}
	return NULL;
}

multimap<int, Widget*> Widget::children_at(float x, float y) {
	multimap<int, Widget*> children;
	for (multimap<int, Widget*>::iterator iter = m_children.begin(); iter != m_children.end(); ++iter) {
		if (iter->second->contains_point(x - get_x(), y - get_y())) {
			children.insert(*iter);
		}
	}
	return children;
}

bool Widget::contains_point(float x, float y) {
	if (x < get_x() - get_width()/2 || x > get_x() + get_width()/2) {
		return false;
	}
	if (y < get_y() - get_height()/2 || y > get_y() + get_height()/2) {
		return false;
	}

	return true;
}

void Widget::change_priority(int old_priority, int new_priority) {
	multimap<int, Widget*> buffer;
	pair<multimap<int, Widget*>::iterator, multimap<int, Widget*>::iterator> range(m_children.equal_range(old_priority));
	for (multimap<int, Widget*>::iterator iter = range.first; iter != range.second; ++iter) {
		buffer.insert(make_pair(new_priority, iter->second));
	}
	m_children.erase(range.first, range.second);
	m_children.insert(buffer.begin(), buffer.end());
}

void Widget::change_priority(Widget* widget, int new_priority) {
	remove_child(widget);
	add_child(widget, new_priority);
}

void Widget::set_x(float x) {
	m_x = x;
}

void Widget::set_y(float y) {
	m_y = y;
}

void Widget::set_width(float w) {
	m_w = w;
}

void Widget::set_height(float h) {
	m_h = h;
}

float Widget::get_x() const {
	return m_x;
}

float Widget::get_y() const {
	return m_y;
}

float Widget::get_width() const {
	return m_w;
}

float Widget::get_height() const {
	return m_h;
}

void Widget::set_drawable(bool drawable) {
	m_drawable = drawable;
}

bool Widget::is_drawable() const {
	return m_drawable;
}

void Widget::focus() {
	//s_focus();
}

void Widget::blur() {
	//s_blur();
}

// TODO propagate events downward

void Widget::mouse_clicked(float x, float y, bool down, int button) {
	Widget* child = top_child_at(x, y);
	if (child != NULL) {
		child->mouse_clicked(x - get_x(), y - get_y(), down, button);
	}

	//s_mouse_clicked(x, y, down, button);
}

void Widget::mouse_moved(float x, float y, float delta_x, float delta_y) {
	multimap<int, Widget*> children = children_at(x, y);
	for (multimap<int, Widget*>::iterator iter = children.begin(); iter != children.end(); ++iter) {
		iter->second->mouse_moved(x - get_x(), y - get_y(), delta_x, delta_y);
	}

	//s_mouse_moved(x, y, delta_x, delta_y);
}

void Widget::keypress(int key, bool down) {
	//s_keypress(key, down);
}

void Widget::draw(DrawContext* ctx) const {
	ctx->translate(get_x(), get_y());
	for (multimap<int, Widget*>::const_iterator iter = m_children.begin(); iter != m_children.end(); ++iter) {
		if (iter->second->is_drawable()) {
			iter->second->draw(ctx);
		}
	}
	ctx->translate(-get_x(), -get_y());
}
