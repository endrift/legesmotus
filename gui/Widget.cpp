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

using namespace LM;
using namespace std;

Widget::Widget(Widget* parent) {
	m_parent = parent;
	if (parent != NULL) {
		parent->add_child(this);
	}

	m_x = 0;
	m_y = 0;
	m_w = 0;
	m_h = 0;

	m_draggable = true;
	m_dragging = false;
}

Widget::~Widget() {
	for (std::list<Widget*>::iterator iter = m_children.begin(); iter != m_children.end(); ++iter) {
		delete *iter;
	}
}

bool Widget::is_dragging() {
	return m_dragging;
}

void Widget::begin_dragging(float initial_x, float initial_y) {
	m_drag_initial_x = initial_x;
	m_drag_initial_y = initial_y;
	m_drag_x = initial_x;
	m_drag_y = initial_y;
	m_dragging = true;
	started_dragging(initial_x, initial_y);
}

void Widget::move_drag(float new_x, float new_y) {
	float delta_x = new_x - m_drag_x;
	float delta_y = new_y - m_drag_y;
	m_drag_x = new_x;
	m_drag_y = new_y;
	dragged(new_x, new_y, delta_x, delta_y);
}

void Widget::end_dragging() {
	m_dragging = false;
	finished_dragging(m_drag_initial_x, m_drag_initial_y, m_drag_x, m_drag_y);
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
	if (!draggable && m_dragging) {
		end_dragging();
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

void Widget::started_dragging(float initial_x, float initial_y) {
	// Nothing to do
}

void Widget::dragged(float current_x, float current_y, float delta_x, float delta_y) {
	set_x(get_x() + delta_x);
	set_y(get_y() + delta_y);
}

void Widget::finished_dragging(float initial_x, float initial_y, float final_x, float final_y) {
	// Nothing to do
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
	// Nothing to do
}

void Widget::blur() {
	// Nothing to do
}

// TODO bubble events

void Widget::mouse_clicked(float x, float y, bool down, int button) {
	if (get_draggable()) {
		if (down && !is_dragging()) {
			begin_dragging(x, y);
		} else if (!down && is_dragging()) {
			end_dragging();
		}
	}
}

void Widget::mouse_moved(float x, float y, float delta_x, float delta_y) {
	if (is_dragging()) {
		move_drag(x, y);
	}
}

void Widget::keypress(int key, bool down) {
	// Nothing to do
}

void Widget::redraw(DrawContext* ctx) {
	ctx->translate(get_x(), get_y());
	ctx->set_draw_color(Color(0.5, 0.5, 0.5));
	ctx->draw_rect(get_width(), get_height());
	ctx->translate(-get_x(), -get_y());
}
