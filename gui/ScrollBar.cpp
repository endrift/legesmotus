/*
 * gui/ScrollBar.cpp
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

#include "ScrollBar.hpp"

using namespace LM;
using namespace std;

ScrollBar::ScrollBar(Widget* parent) : Widget(parent) {
	m_is_horizontal = false;
	m_last_mouse_pos.x = numeric_limits<float>::max();
	m_last_mouse_pos.y = numeric_limits<float>::max();
	m_virtual_size = 0.0f;
	m_scroll_fraction = 0.0f;
	m_border_radius = 0;
	m_border_padding = 0;
	m_max_corner_sections = 16;
	m_arrow_size = 32.0;
	m_arrow_icon_major_axis = 10.0;
	m_arrow_icon_minor_axis = 8.0;
	m_state = STATE_NORMAL;
	m_scroll_pixels_per_milli = 0.2;
	m_callback_object = NULL;
	m_change_on_mousedown = false;
	m_change_on_hover = false;
	m_use_parent_size = false;
	m_parent_size = 0.0f;
}

ScrollBar::~ScrollBar() {
}

float ScrollBar::get_parent_size_transform() const {
	if (m_is_horizontal) {
		return m_parent_size / (get_width() - 2 * m_arrow_size - 4 * m_border_padding);
	} else {
		return m_parent_size / (get_height() - 2 * m_arrow_size - 4 * m_border_padding);
	}
}

float ScrollBar::get_internal_size() const {
	if (m_use_parent_size) {
		return m_parent_size;
	}
	
	if (m_is_horizontal) {
		return get_width() - 2 * m_arrow_size - 4 * m_border_padding;
	} else {
		return get_height() - 2 * m_arrow_size - 4 * m_border_padding;
	}
}

float ScrollBar::get_drag_bar_length() const {
	return min((get_internal_size() / m_virtual_size) * get_internal_size(), get_internal_size());
}

float ScrollBar::get_max_scroll_fraction() const {
	return 1.0 - get_drag_bar_length()/get_internal_size();
}

float ScrollBar::get_drag_bar_center() const {
	float internal_size = get_internal_size();
	return min(get_max_scroll_fraction(), m_scroll_fraction) * internal_size - internal_size/2 + get_drag_bar_length()/2;
}

bool ScrollBar::point_is_in_top(float x, float y) const {
	float my_x = get_x();
	float my_y = get_y();
	float my_width = get_width();
	float my_height = get_height();
	
	if (!m_is_horizontal && x > my_x - my_width/2 && x < my_x + my_width/2 && 
		y > my_y - my_height/2 && y < my_y - my_height/2 + m_arrow_size) {
		return true;
	} else if (m_is_horizontal && x > my_x - my_width/2 && x < my_x - my_width/2 + m_arrow_size &&
		y > my_y - my_height/2 && y < my_y + my_height/2) {
		return true;
	}
	
	return false;
}

bool ScrollBar::point_is_in_drag_bar(float x, float y) const {
	float virtual_dimension = get_drag_bar_length();
	float drag_center = get_drag_bar_center();
	float my_x = get_x();
	float my_y = get_y();
	float my_width = get_width();
	float my_height = get_height();
	if (m_is_horizontal) {
		return (y > my_y - my_height/2 && x < my_y + my_height/2 && 
			x > my_x + drag_center - virtual_dimension/2 &&
			x < my_x + drag_center + virtual_dimension/2);
	} else {
		return (x > my_x - my_width/2 && x < my_x + my_width/2 && 
			y > my_y + drag_center - virtual_dimension/2 &&
			y < my_y + drag_center + virtual_dimension/2);
	}
	
	return false;
}

bool ScrollBar::point_is_in_bottom(float x, float y) const {
	float my_x = get_x();
	float my_y = get_y();
	float my_width = get_width();
	float my_height = get_height();
	
	if (!m_is_horizontal && x > my_x - my_width/2 && x < my_x + my_width/2 && 
		y > my_y + my_height/2 - m_arrow_size && y < my_y + my_height/2) {
		return true;
	} else if (m_is_horizontal && x > my_x + my_width/2 - m_arrow_size && x < my_x + my_width/2 &&
		y > my_y - my_height/2 && y < my_y + my_height/2) {
		return true;
	}
	
	return false;
}

float ScrollBar::get_scroll_fraction() const {
	return m_scroll_fraction;
}

float ScrollBar::get_virtual_size() const {
	return m_virtual_size;
}

bool ScrollBar::is_horizontal() const {
	return m_is_horizontal;
}

void ScrollBar::set_state(ScrollBarState state) {
	m_state = state;
}

ScrollBar::ScrollBarState ScrollBar::get_state() const {
	return m_state;
}

void ScrollBar::set_hover_color(const Color& c, ColorType type) {
	ASSERT((unsigned long) type < COLOR_MAX);
	m_hover_colors[type] = c;
}

const Color& ScrollBar::get_hover_color(ColorType type) const {
	ASSERT((unsigned long) type < COLOR_MAX);
	return m_hover_colors[type];
}

void ScrollBar::set_inactive_color(const Color& c, ColorType type) {
	ASSERT((unsigned long) type < COLOR_MAX);
	m_inactive_colors[type] = c;
}

const Color& ScrollBar::get_inactive_color(ColorType type) const {
	ASSERT((unsigned long) type < COLOR_MAX);
	return m_inactive_colors[type];
}

void ScrollBar::set_pressed_color(const Color& c, ColorType type) {
	ASSERT((unsigned long) type < COLOR_MAX);
	m_pressed_colors[type] = c;
}

const Color& ScrollBar::get_pressed_color(ColorType type) const {
	ASSERT((unsigned long) type < COLOR_MAX);
	return m_pressed_colors[type];
}

void ScrollBar::set_change_on_pressed(bool change) {
	m_change_on_mousedown = change;
}

void ScrollBar::set_change_on_hover(bool change) {
	m_change_on_hover = change;
}

void ScrollBar::set_use_parent_size(bool use_parent_size) {
	m_use_parent_size = use_parent_size;
}

void ScrollBar::set_callback_object(ScrollBarCallback* callback_object) {
	m_callback_object = callback_object;
}

void ScrollBar::set_horizontal(bool horizontal) {
	m_is_horizontal = horizontal;
}

void ScrollBar::set_virtual_size(float size) {
	m_virtual_size = max(0.0f, size);
}

void ScrollBar::set_parent_size(float size) {
	m_parent_size = max(0.0f, size);
}

void ScrollBar::set_scroll_fraction(float fraction) {
	if (fraction < 0.0f) {
		fraction = 0.0f;
	} else if (fraction > get_max_scroll_fraction()) {
		fraction = get_max_scroll_fraction();
	}
	
	m_scroll_fraction = fraction;
	
	if (m_callback_object != NULL) {
		m_callback_object->scroll_fraction_changed(get_id(), this, m_scroll_fraction);
	}
}

void ScrollBar::set_border_radius(float radius) {
	if (radius < 0) {
		radius = 0;
	}
	
	m_border_radius = radius;
}

void ScrollBar::set_border_padding(float padding) {
	if (padding < 0) {
		padding = 0;
	}
	
	m_border_padding = padding;
}

void ScrollBar::set_border_max_roundness(int sections) {
	if (sections < 1) {
		sections = 1;
	}
	
	m_max_corner_sections = sections;
}

void ScrollBar::set_arrow_size(float pixels) {
	if (pixels < 0) {
		pixels = 0;
	}
	
	m_arrow_size = pixels;
}

void ScrollBar::set_arrow_icon_major_axis(float pixels) {
	if (pixels < 0) {
		pixels = 0;
	}
	
	m_arrow_icon_major_axis = pixels;
}

void ScrollBar::set_arrow_icon_minor_axis(float pixels) {
	if (pixels < 0) {
		pixels = 0;
	}
	
	m_arrow_icon_minor_axis = pixels;
}

void ScrollBar::set_scroll_speed(float pixels_per_millisecond) {
	if (pixels_per_millisecond < 1.0f) {
		pixels_per_millisecond = 1.0f;
	}
	m_scroll_pixels_per_milli = pixels_per_millisecond;
}

void ScrollBar::set_arrow_color(const Color& c) {
	m_arrow_color = c;
}

void ScrollBar::private_mouse_clicked(bool child_handled, float x, float y, bool down, int button) {
	if (down && m_state == STATE_NORMAL) {
		if (point_is_in_top(x, y)) {
			m_state = STATE_TOP_PRESSED;
		} else if (point_is_in_bottom(x, y)) {
			m_state = STATE_BOTTOM_PRESSED;
		} else if (point_is_in_drag_bar(x, y)) {
			m_state = STATE_MIDDLE_DRAGGED;
			if (m_is_horizontal) {
				m_drag_start_pos = x;
			} else {
				m_drag_start_pos = y;
			}
			m_drag_start_percent = m_scroll_fraction;
		}
	}
	
	if (!down && m_state != STATE_INACTIVE) {
		m_state = STATE_NORMAL;
	}
}

void ScrollBar::private_mouse_moved(bool child_handled, float x, float y, float delta_x, float delta_y) {
	m_last_mouse_pos.x = x;
	m_last_mouse_pos.y = y;
	if (m_state == STATE_MIDDLE_DRAGGED) {
		if (m_is_horizontal) {
			set_scroll_fraction(m_drag_start_percent + (x - m_drag_start_pos) / (get_width() - 2*m_arrow_size));
		} else {
			set_scroll_fraction(m_drag_start_percent + (y - m_drag_start_pos) / (get_height() - 2*m_arrow_size));
		}
	}
}

void ScrollBar::update(uint64_t timediff) {
	if (m_state == STATE_TOP_PRESSED) {
		set_scroll_fraction(m_scroll_fraction - (m_scroll_pixels_per_milli/m_virtual_size) * timediff);
	} else if (m_state == STATE_BOTTOM_PRESSED) {
		set_scroll_fraction(m_scroll_fraction + (m_scroll_pixels_per_milli/m_virtual_size) * timediff);
	}
}

void ScrollBar::draw(DrawContext* ctx) const {
	ctx->push_transform();
	
	ctx->translate(get_x(), get_y());
	
	if (m_state == STATE_INACTIVE) {
		ctx->set_draw_color(get_inactive_color(COLOR_SECONDARY));
	} else if (m_change_on_hover && contains_point(m_last_mouse_pos.x, m_last_mouse_pos.y)) {
		ctx->set_draw_color(get_hover_color(COLOR_SECONDARY));
	} else {
		ctx->set_draw_color(get_color(COLOR_SECONDARY));
	}
	
	// TODO: Make background pretty?
	ctx->draw_roundrect_fill(get_width(), get_height(), m_border_radius, m_max_corner_sections);
	
	// TODO: Make foreground pretty?
	if (m_state == STATE_INACTIVE) {
		ctx->set_draw_color(get_inactive_color(COLOR_PRIMARY));
	} else if (m_state == STATE_TOP_PRESSED && m_change_on_mousedown) {
		ctx->set_draw_color(get_pressed_color(COLOR_PRIMARY));
	} else if (m_change_on_hover && point_is_in_top(m_last_mouse_pos.x, m_last_mouse_pos.y)) {
		ctx->set_draw_color(get_hover_color(COLOR_PRIMARY));
	} else {
		ctx->set_draw_color(get_color(COLOR_PRIMARY));
	}
	
	float vertices[6];
	// Draw arrows
	if (m_is_horizontal) {
		ctx->translate(m_arrow_size/2 - get_width()/2 + m_border_padding, 0);
		ctx->draw_roundrect_fill(m_arrow_size, get_height() - 2 * m_border_padding, m_border_radius, m_max_corner_sections);
		ctx->set_draw_color(m_arrow_color);
		vertices[0] = -1.0 * m_arrow_icon_major_axis/2;
		vertices[1] = 0;
		vertices[2] = m_arrow_icon_major_axis/2;
		vertices[3] = -1.0 * m_arrow_icon_minor_axis/2;
		vertices[4] = m_arrow_icon_major_axis/2;
		vertices[5] = m_arrow_icon_minor_axis/2;
		ctx->draw_polygon_fill(vertices, 3);
		if (m_state == STATE_INACTIVE) {
			ctx->set_draw_color(get_inactive_color(COLOR_PRIMARY));
		} else if (m_state == STATE_BOTTOM_PRESSED && m_change_on_mousedown) {
			ctx->set_draw_color(get_pressed_color(COLOR_PRIMARY));
		} else if (m_change_on_hover && point_is_in_bottom(m_last_mouse_pos.x, m_last_mouse_pos.y)) {
			ctx->set_draw_color(get_hover_color(COLOR_PRIMARY));
		} else {
			ctx->set_draw_color(get_color(COLOR_PRIMARY));
		}
		ctx->translate(get_width() - m_arrow_size - 2 * m_border_padding, 0);
		ctx->draw_roundrect_fill(m_arrow_size, get_height() - 2 * m_border_padding, m_border_radius, m_max_corner_sections);
		ctx->set_draw_color(m_arrow_color);
		vertices[0] = m_arrow_icon_major_axis/2;
		vertices[1] = 0;
		vertices[2] = -1.0 * m_arrow_icon_major_axis/2;
		vertices[3] = -1.0 * m_arrow_icon_minor_axis/2;
		vertices[4] = -1.0 * m_arrow_icon_major_axis/2;
		vertices[5] = m_arrow_icon_minor_axis/2;
		ctx->draw_polygon_fill(vertices, 3);
		ctx->translate(m_arrow_size/2-get_width()/2 + m_border_padding, 0);
	} else {
		ctx->translate(0, m_arrow_size/2 - get_height()/2 + m_border_padding);
		ctx->draw_roundrect_fill(get_width() - 2 * m_border_padding, m_arrow_size, m_border_radius, m_max_corner_sections);
		ctx->set_draw_color(m_arrow_color);
		vertices[0] = 0;
		vertices[1] = -1.0 * m_arrow_icon_major_axis/2;
		vertices[2] = -1.0 * m_arrow_icon_minor_axis/2;
		vertices[3] = m_arrow_icon_major_axis/2;
		vertices[4] = m_arrow_icon_minor_axis/2;
		vertices[5] = m_arrow_icon_major_axis/2;
		ctx->draw_polygon_fill(vertices, 3);
		if (m_state == STATE_INACTIVE) {
			ctx->set_draw_color(get_inactive_color(COLOR_PRIMARY));
		} else if (m_state == STATE_BOTTOM_PRESSED && m_change_on_mousedown) {
			ctx->set_draw_color(get_pressed_color(COLOR_PRIMARY));
		} else if (m_change_on_hover && point_is_in_bottom(m_last_mouse_pos.x, m_last_mouse_pos.y)) {
			ctx->set_draw_color(get_hover_color(COLOR_PRIMARY));
		} else {
			ctx->set_draw_color(get_color(COLOR_PRIMARY));
		}
		ctx->translate(0, get_height() - m_arrow_size - 2 * m_border_padding);
		ctx->draw_roundrect_fill(get_width() - 2 * m_border_padding, m_arrow_size, m_border_radius, m_max_corner_sections);
		ctx->set_draw_color(m_arrow_color);
		vertices[0] = 0;
		vertices[1] = m_arrow_icon_major_axis/2;
		vertices[2] = -1.0 * m_arrow_icon_minor_axis/2;
		vertices[3] = -1.0 * m_arrow_icon_major_axis/2;
		vertices[4] = m_arrow_icon_minor_axis/2;
		vertices[5] = -1.0 * m_arrow_icon_major_axis/2;
		ctx->draw_polygon_fill(vertices, 3);
		ctx->translate(0, m_arrow_size/2-get_height()/2 + m_border_padding);
	}
	
	if (m_state == STATE_INACTIVE) {
		ctx->set_draw_color(get_inactive_color(COLOR_PRIMARY));
	} else if (m_state == STATE_MIDDLE_DRAGGED && m_change_on_mousedown) {
		ctx->set_draw_color(get_pressed_color(COLOR_PRIMARY));
	} else if (m_change_on_hover && point_is_in_drag_bar(m_last_mouse_pos.x, m_last_mouse_pos.y)) {
		ctx->set_draw_color(get_hover_color(COLOR_PRIMARY));
	} else {
		ctx->set_draw_color(get_color(COLOR_PRIMARY));
	}
	
	// Draw bar
	float virtual_dimension;
	float internal_size;
	float max_scroll_fraction;
	float draw_compression;
	if (m_is_horizontal) {
		if (m_use_parent_size) {
			internal_size = m_parent_size;
		} else {
			internal_size = get_width() - 2 * m_arrow_size - 4 * m_border_padding;
		}
		virtual_dimension = min((internal_size / m_virtual_size) * internal_size, internal_size);
		max_scroll_fraction = 1.0 - virtual_dimension/internal_size;
		draw_compression = (get_width() - 2 * m_arrow_size - 4 * m_border_padding) / internal_size;
		ctx->translate(draw_compression * (min(max_scroll_fraction, m_scroll_fraction) * internal_size - internal_size/2 + virtual_dimension/2), 0);
		ctx->draw_roundrect_fill(draw_compression * virtual_dimension, get_height() - 2 * m_border_padding, m_border_radius, m_max_corner_sections);
	} else {
		if (m_use_parent_size) {
			internal_size = m_parent_size;
		} else {
			internal_size = get_height() - 2 * m_arrow_size - 4 * m_border_padding;
		}
		
		virtual_dimension = min((internal_size / m_virtual_size) * internal_size, internal_size);
		max_scroll_fraction = 1.0 - virtual_dimension/internal_size;
		draw_compression = (get_height() - 2 * m_arrow_size - 4 * m_border_padding) / internal_size;
		ctx->translate(0, draw_compression * (min(max_scroll_fraction, m_scroll_fraction) * internal_size - internal_size/2 + virtual_dimension/2));
		ctx->draw_roundrect_fill(get_width() - 2 * m_border_padding, draw_compression * virtual_dimension, m_border_radius, m_max_corner_sections);
	}
	
	ctx->pop_transform();

	Widget::draw(ctx);
}
