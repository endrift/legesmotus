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

	m_dragging = false;
	set_draggable(true);
}

Widget::~Widget() {
	for (std::list<Widget*>::iterator iter = m_children.begin(); iter != m_children.end(); ++iter) {
		delete *iter;
	}
}

bool Widget::is_dragging() {
	return m_dragging;
}

void Widget::drag_begin(float initial_x, float initial_y) {
	m_drag_initial_x = initial_x;
	m_drag_initial_y = initial_y;
	m_drag_x = initial_x;
	m_drag_y = initial_y;
	m_dragging = true;
	s_drag_begin(initial_x, initial_y);
}

void Widget::drag_move(float new_x, float new_y) {
	m_drag_x = new_x;
	m_drag_y = new_y;
	s_drag_move(new_x, new_y);
}

void Widget::drag_end() {
	m_dragging = false;
	s_drag_end(m_drag_x, m_drag_y);
}

void Widget::drag_reloc_begin(float initial_x, float initial_y) {
	m_drag_local_x = initial_x - get_x();
	m_drag_local_y = initial_y - get_y();
}

void Widget::drag_reloc(float current_x, float current_y) {
	set_x(current_x - m_drag_local_x);
	set_y(current_y - m_drag_local_y);
}

void Widget::add_child(Widget* child) {
	if (child != NULL) {
		m_children.push_front(child);
	}
}

void Widget::remove_child(Widget* child) {
	if (child != NULL) {
		m_children.remove(child);
	}
}

void Widget::set_draggable(bool draggable) {
	m_draggable = draggable;
	if (draggable) {
		s_drag_begin.connect(bind(&Widget::drag_reloc_begin, this, _1, _2));
		s_drag_move.connect(bind(&Widget::drag_reloc, this, _1, _2));
	} else {
		if (m_dragging) {
			drag_end();
		}

		s_drag_begin.disconnect(bind(&Widget::drag_reloc_begin, this));
		s_drag_move.disconnect(bind(&Widget::drag_reloc, this));
	}
}

bool Widget::get_draggable() const {
	return m_draggable;
}

float Widget::get_drag_initial_x() const {
	return m_drag_initial_x;
}

float Widget::get_drag_initial_y() const {
	return m_drag_initial_y;
}

float Widget::get_drag_x() const {
	return m_drag_x;
}

float Widget::get_drag_y() const {
	return m_drag_y;
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

const list<Widget*>& Widget::get_children() {
	return m_children;
}

Widget* Widget::child_at(float x, float y) {
	for (list<Widget*>::iterator iter = m_children.begin(); iter != m_children.end(); ++iter) {
		if ((*iter)->contains_point(x, y)) {
			return *iter;
		}
	}
	return NULL;
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

void Widget::focus() {
	s_focus();
}

void Widget::blur() {
	s_blur();
}

// TODO propagate events downward

void Widget::mouse_clicked(float x, float y, bool down, int button) {
	if (get_draggable()) {
		if (down && !is_dragging() && contains_point(x, y)) {
			drag_begin(x, y);
		} else if (!down && is_dragging()) {
			drag_end();
		}
	}

	s_mouse_moved(x, y, down, button);
}

void Widget::mouse_moved(float x, float y, float delta_x, float delta_y) {
	if (is_dragging()) {
		drag_move(x, y);
	}

	s_mouse_moved(x, y, delta_x, delta_y);
}

void Widget::keypress(int key, bool down) {
	s_keypress(key, down);
}

void Widget::redraw(DrawContext* ctx) {
	ctx->translate(get_x(), get_y());
	ctx->set_draw_color(Color(0.5, 0.5, 0.5));
	ctx->draw_rect_fill(get_width(), get_height());
	ctx->translate(-get_x(), -get_y());
}
