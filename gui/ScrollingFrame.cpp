/*
 * gui/ScrollingFrame.cpp
 *
 * This file is part of Leges Motus, a networked, 2D shooter set in zero gravity.
 * 
 * Copyright 2009-2011 Andrew Ayer, Nathan Partlan, Jeffrey Pfau
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

#include "ScrollingFrame.hpp"

using namespace LM;
using namespace std;

ScrollingFrame::ScrollingFrame(Widget* parent) : BackgroundFrame(parent) {
	m_right_scrollbar = new ScrollBar(this);
	m_right_scrollbar->set_width(20);
	m_right_scrollbar->set_height(0);
	m_right_scrollbar->set_x(0);
	m_right_scrollbar->set_y(0);
	m_right_scrollbar->set_horizontal(false);
	m_right_scrollbar->set_border_radius(10.0f);
	m_right_scrollbar->set_border_padding(2.0f);
	m_right_scrollbar->set_virtual_size(0);
	m_right_scrollbar->set_callback_object(this);
	m_right_scrollbar->set_change_on_pressed(true);
	m_right_scrollbar->set_change_on_hover(true);
	m_right_scrollbar->set_state(ScrollBar::STATE_INACTIVE);
	// Must not be drawn normally to avoid being scrolled with other widgets
	m_right_scrollbar->set_drawable(false);
	m_right_scrollbar->set_use_parent_size(true);
	m_right_scrollbar->set_receives_input(false);
	
	m_bottom_scrollbar = new ScrollBar(this);
	m_bottom_scrollbar->set_width(0);
	m_bottom_scrollbar->set_height(20);
	m_bottom_scrollbar->set_x(0);
	m_bottom_scrollbar->set_y(0);
	m_bottom_scrollbar->set_horizontal(true);
	m_bottom_scrollbar->set_border_radius(10.0f);
	m_bottom_scrollbar->set_border_padding(2.0f);
	m_bottom_scrollbar->set_virtual_size(0);
	m_bottom_scrollbar->set_callback_object(this);
	m_bottom_scrollbar->set_change_on_pressed(true);
	m_bottom_scrollbar->set_change_on_hover(true);
	m_bottom_scrollbar->set_state(ScrollBar::STATE_INACTIVE);
	m_bottom_scrollbar->set_drawable(false);
	m_bottom_scrollbar->set_use_parent_size(true);
	m_bottom_scrollbar->set_receives_input(false);
}

ScrollingFrame::~ScrollingFrame() {
}

void ScrollingFrame::private_mouse_clicked(bool child_handled, float x, float y, bool down, int button) {
	// Check if it should be passed to our scrollbars
	if (down) {
		if (m_bottom_scrollbar->contains_point(x - get_x(), y - get_y())) {
			m_bottom_scrollbar->mouse_clicked(x - get_x(), y - get_y(), down, button);
		}
	
		if (m_right_scrollbar->contains_point(x - get_x(), y - get_y())) {
			m_right_scrollbar->mouse_clicked(x - get_x(), y - get_y(), down, button);
		}
	} else {
		m_bottom_scrollbar->mouse_clicked(x - get_x(), y - get_y(), down, button);
		m_right_scrollbar->mouse_clicked(x - get_x(), y - get_y(), down, button);
	}
}

void ScrollingFrame::private_mouse_moved(bool child_handled, float x, float y, float delta_x, float delta_y) {
	// Pass it to our scrollbars
	m_bottom_scrollbar->mouse_moved(x - get_x(), y - get_y(), delta_x, delta_y);
	m_right_scrollbar->mouse_moved(x - get_x(), y - get_y(), delta_x, delta_y);
}

float ScrollingFrame::get_usable_width() const {
	float internal_width = get_internal_width();
	if (m_can_scroll_vert) {
		internal_width -= m_right_scrollbar->get_width();
	}
	
	return internal_width;
}

float ScrollingFrame::get_usable_height() const {
	float internal_height = get_internal_height();
	if (m_can_scroll_horiz) {
		internal_height -= m_bottom_scrollbar->get_height();
	}
	
	return internal_height;
}

Point ScrollingFrame::get_relative_point(float x, float y) {
	return Point(x - get_x() - m_bottom_scrollbar->get_scroll_fraction() * -1 * m_bottom_scrollbar->get_virtual_size(), 
			y - get_y() - m_right_scrollbar->get_scroll_fraction() * -1 * m_right_scrollbar->get_virtual_size());
}

void ScrollingFrame::set_width(float width) {
	BackgroundFrame::set_width(width);
	if (m_can_scroll_vert) {
		m_bottom_scrollbar->set_x(-m_right_scrollbar->get_width()/2 - 2);
	}
	m_right_scrollbar->set_x(get_internal_width()/2 - m_right_scrollbar->get_width()/2);
	m_bottom_scrollbar->set_width(get_usable_width() - 4);
	m_bottom_scrollbar->set_parent_size(get_usable_width());
}

void ScrollingFrame::set_height(float height) {
	BackgroundFrame::set_height(height);
	m_bottom_scrollbar->set_y(get_internal_height()/2 - m_bottom_scrollbar->get_height()/2);
	m_right_scrollbar->set_height(get_internal_height() - 2);
	m_right_scrollbar->set_parent_size(get_usable_height());
}

void ScrollingFrame::set_bar_minor_dimension(float size) {
	if (size < 0.0f) {
		size = 0.0f;
	}
	m_right_scrollbar->set_width(size);
	m_bottom_scrollbar->set_height(size);
}

void ScrollingFrame::set_bar_border_radius(float size) {
	m_right_scrollbar->set_border_radius(size);
	m_bottom_scrollbar->set_border_radius(size);
}

void ScrollingFrame::set_bar_padding(float padding) {
	m_right_scrollbar->set_border_padding(padding);
	m_bottom_scrollbar->set_border_padding(padding);
}

void ScrollingFrame::set_bar_arrow_size(float size) {
	m_right_scrollbar->set_arrow_size(size);
	m_bottom_scrollbar->set_arrow_size(size);
}

void ScrollingFrame::set_bar_arrow_minor_axis(float size) {
	m_right_scrollbar->set_arrow_icon_minor_axis(size);
	m_bottom_scrollbar->set_arrow_icon_minor_axis(size);
}

void ScrollingFrame::set_bar_arrow_major_axis(float size) {
	m_right_scrollbar->set_arrow_icon_major_axis(size);
	m_bottom_scrollbar->set_arrow_icon_major_axis(size);
}

void ScrollingFrame::set_bar_arrow_color(const Color& color) {
	m_right_scrollbar->set_arrow_color(color);
	m_bottom_scrollbar->set_arrow_color(color);
}

void ScrollingFrame::set_bar_scroll_speed(float pixels_per_millisecond) {
	m_right_scrollbar->set_scroll_speed(pixels_per_millisecond);
	m_bottom_scrollbar->set_scroll_speed(pixels_per_millisecond);
}

void ScrollingFrame::set_bar_color(const Color& c, ColorType type) {
	m_bottom_scrollbar->set_color(c, type);
	m_right_scrollbar->set_color(c, type);
}

void ScrollingFrame::set_bar_inactive_color(const Color& c, ColorType type) {
	m_bottom_scrollbar->set_inactive_color(c, type);
	m_right_scrollbar->set_inactive_color(c, type);
}
		
void ScrollingFrame::set_bar_hover_color(const Color& c, ColorType type) {
	m_bottom_scrollbar->set_hover_color(c, type);
	m_right_scrollbar->set_hover_color(c, type);
}

void ScrollingFrame::set_bar_pressed_color(const Color& c, ColorType type) {
	m_bottom_scrollbar->set_pressed_color(c, type);
	m_right_scrollbar->set_pressed_color(c, type);
}

void ScrollingFrame::set_bar_change_on_pressed(bool change) {
	m_bottom_scrollbar->set_change_on_pressed(change);
	m_right_scrollbar->set_change_on_pressed(change);
}

void ScrollingFrame::set_bar_change_on_hover(bool change) {
	m_bottom_scrollbar->set_change_on_hover(change);
	m_right_scrollbar->set_change_on_hover(change);
}

void ScrollingFrame::set_can_scroll(bool can_scroll_horiz, bool can_scroll_vert) {
	m_can_scroll_horiz = can_scroll_horiz;
	m_can_scroll_vert = can_scroll_vert;
	
	if (!can_scroll_horiz) {
		m_bottom_scrollbar->set_scroll_fraction(0);
	}
	
	if (!can_scroll_vert) {
		m_right_scrollbar->set_scroll_fraction(0);
	}
}

void ScrollingFrame::scroll_fraction_changed(int bar_id, ScrollBar* scroll_bar, float fraction) {
}

void ScrollingFrame::update(uint64_t timediff) {
	Iterator<pair<int, Widget*> > iter = list_children();
	float virtual_vert_size = 0;
	float virtual_horiz_size = 0;
	while(iter.has_more()) {
		Widget* widget = (iter.next()).second;
		if (widget == m_bottom_scrollbar || widget == m_right_scrollbar) {
			continue;
		}
		if (m_can_scroll_vert && virtual_vert_size < get_internal_height()/2 + widget->get_y() + widget->get_height()/2) {
			virtual_vert_size = get_internal_height()/2 + widget->get_y() + widget->get_height()/2;
		}
		if (m_can_scroll_horiz && virtual_horiz_size < get_internal_width()/2 + widget->get_x() + widget->get_width()/2) {
			virtual_horiz_size = get_internal_width()/2 + widget->get_x() + widget->get_width()/2;
		}
	}
	
	m_bottom_scrollbar->set_virtual_size(virtual_horiz_size);
	m_right_scrollbar->set_virtual_size(virtual_vert_size);
	
	if (m_bottom_scrollbar->get_state() == ScrollBar::STATE_INACTIVE && virtual_horiz_size > get_usable_width()) {
		m_bottom_scrollbar->set_state(ScrollBar::STATE_NORMAL);
	}
	
	if (m_right_scrollbar->get_state() == ScrollBar::STATE_INACTIVE && virtual_vert_size > get_usable_height()) {
		m_right_scrollbar->set_state(ScrollBar::STATE_NORMAL);
	}
	
	BackgroundFrame::update(timediff);
}

void ScrollingFrame::draw_internals(DrawContext* ctx) const {
	ctx->push_transform();
	
	ctx->translate(m_bottom_scrollbar->get_scroll_fraction() * -1 * m_bottom_scrollbar->get_virtual_size(),
			m_right_scrollbar->get_scroll_fraction() * -1 * m_right_scrollbar->get_virtual_size());

	BackgroundFrame::draw_internals(ctx);
	
	ctx->pop_transform();
}	

void ScrollingFrame::draw(DrawContext* ctx) const {	
	BackgroundFrame::draw(ctx);
	
	ctx->push_transform();
	ctx->translate(get_x(), get_y());
	
	if (m_can_scroll_horiz) {
		m_bottom_scrollbar->draw(ctx);
	}
	
	if (m_can_scroll_vert) {
		m_right_scrollbar->draw(ctx);
	}
	
	ctx->pop_transform();
}
