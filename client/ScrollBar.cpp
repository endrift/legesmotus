/*
 * client/ScrollBar.cpp
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

#include "ScrollBar.hpp"
#include "ScrollArea.hpp"
#include <cmath>

using namespace LM;

const double ScrollBar::DEFAULT_AUTOSCROLL = 50.0;

ScrollBar::ScrollBar(bool horizontal, ScrollArea* area) : m_bg(5,SCROLL_WIDTH) {
	m_bg.set_corner_radius(SCROLL_WIDTH*0.5);
	m_bg.set_border_color(Color::WHITE);
	m_bg.set_border_width(2);
	m_bg.set_border_collapse(true);
	m_bg.set_row_height(0, SCROLL_WIDTH);
	m_bg.set_row_height(4, SCROLL_WIDTH);
	m_bg.set_center_x(SCROLL_WIDTH*0.5);
	m_horizontal = horizontal;

	m_updated = false;
	if (area != NULL) {
		if (m_horizontal) {
			area->horiz_relink(this);
		} else {
			area->vert_relink(this);
		}
	} else {
		m_linked = NULL;
	}
	m_progress = 0.0;
	set_height(SCROLL_WIDTH*5);
	set_scrollbar_length(SCROLL_WIDTH);
	m_pressed = NO_WIDGET;

	m_scroll_speed = 1.0;
	m_track_speed = 3.0;

	if (m_horizontal) {
		m_bg.set_rotation(-90);
	}
}

ScrollBar::ScrollBar(const ScrollBar& other) : m_bg(other.m_bg) {
	m_updated = false;
	set_height(other.m_height);
	set_scrollbar_length(other.m_scrollbar_height);
	m_horizontal = other.m_horizontal;
	m_linked = NULL;
	m_progress = other.m_progress;
	m_pressed = NO_WIDGET;
	m_scroll_speed = other.m_scroll_speed;
	m_track_speed = other.m_track_speed;
}

ScrollBar* ScrollBar::clone() const {
	return new ScrollBar(*this);
}

void ScrollBar::autoscroll(double scale) {
	double amount = 0;
	switch (m_pressed) {
	case TOP_BUTTON:
		amount = -m_scroll_speed;
		break;
	case TOP_TRACK:
		amount = -m_track_speed;
		break;
	case BOTTOM_TRACK:
		amount = m_track_speed;
		break;
	case BOTTOM_BUTTON:
		amount = m_scroll_speed;
		break;
	default: break;
	}

	if (m_linked != NULL) {
		if (m_horizontal) {
			m_linked->horiz_scroll_pixels(amount*20*scale/m_height);
		} else {
			m_linked->vert_scroll_pixels(amount*20*scale/m_height);
		}
	} else {
		scroll(amount*scale/m_height);
	}
}

void ScrollBar::set_width(double width) {
	if (m_horizontal) {
		m_height = width;
		m_bg.set_center_y(width*0.5);
		set_scroll_progress(m_progress);
	}
}

void ScrollBar::set_height(double height) {
	if (!m_horizontal) {
		m_height = height;
		m_bg.set_center_y(height*0.5);
		set_scroll_progress(m_progress);
	}
}

void ScrollBar::set_length(double length) {
	m_height = length;
	m_bg.set_center_y(length*0.5);
	set_scroll_progress(m_progress);
}

double ScrollBar::get_width() const {
	if (m_horizontal) {
		return m_height;
	} else {
		return SCROLL_WIDTH;
	}
}

double ScrollBar::get_height() const {
	if (m_horizontal) {
		return SCROLL_WIDTH;
	} else {
		return m_height;
	}
}

double ScrollBar::get_length() const {
	return m_height;
}

void ScrollBar::mouse_button_event(const SDL_MouseButtonEvent& event) {
	if (event.state == SDL_RELEASED) {
		m_pressed = NO_WIDGET;
		return;
	}

	double x;
	double y;
	int ex;
	int ey;
	if (m_horizontal) {
		y = get_x() - get_center_x();
		x = get_y() - get_center_y();
		ey = event.x;
		ex = event.y;
	} else {
		x = get_x() - get_center_x();
		y = get_y() - get_center_y();
		ex = event.x;
		ey = event.y;
	}

	if (ex < (x - SCROLL_WIDTH*0.5) || ex > (x + SCROLL_WIDTH*0.5)) {
		return;
	} else if(ey < (y - m_height*0.5) || ey > (y + m_height*0.5)) {
		return;
	}

	if(ey < (y - (m_height*0.5 - SCROLL_WIDTH))) {
		m_pressed = TOP_BUTTON;
		autoscroll(DEFAULT_AUTOSCROLL);
	} else if(ey > (y + (m_height*0.5 - SCROLL_WIDTH))) {
		m_pressed = BOTTOM_BUTTON;
		autoscroll(DEFAULT_AUTOSCROLL);
	} else if(ey < (y - (m_height*0.5 - SCROLL_WIDTH) +
			m_bg.get_row_height(1))) {
		m_pressed = TOP_TRACK;
		autoscroll(DEFAULT_AUTOSCROLL);
	} else if(ey > (y + (m_height*0.5 - SCROLL_WIDTH) -
			m_bg.get_row_height(3))) {
		m_pressed = BOTTOM_TRACK;
		autoscroll(DEFAULT_AUTOSCROLL);
	} else {
		m_pressed = TRACKER;
		m_grab_y = ey - m_progress*(m_height-SCROLL_WIDTH*2-m_scrollbar_height);
	}
}

void ScrollBar::mouse_motion_event(const SDL_MouseMotionEvent& event) {
	if (m_pressed != TRACKER) {
		return;
	}
	double scroll_denominator = m_height-SCROLL_WIDTH*2-m_scrollbar_height;
	if(scroll_denominator == 0) {
		set_scroll_progress(0);
	} else {
		if (m_horizontal) {
			set_scroll_progress((event.x - m_grab_y)/scroll_denominator);
		} else {
			set_scroll_progress((event.y - m_grab_y)/scroll_denominator);
		}
	}
}

void ScrollBar::set_scroll_progress(double amount) {
	if (amount < 0) {
		amount = 0;
	} else if (amount > 1) {
		amount = 1;
	}

	m_progress = amount;
	m_bg.set_row_height(1, (m_height-SCROLL_WIDTH*2-m_scrollbar_height)*amount);
	m_bg.set_row_height(3, (m_height-SCROLL_WIDTH*2-m_scrollbar_height)*(1-amount));

	if(m_linked != NULL && !m_updated) {
		m_updated = true;
		if (m_horizontal) {
			m_linked->set_horiz_scroll_progress(amount);
		} else {
			m_linked->set_vert_scroll_progress(amount);
		}
		m_updated = false;
	}
}

void ScrollBar::scroll(double amount) {
	set_scroll_progress(m_progress + amount);
}

double ScrollBar::get_scroll_progress() const {
	return m_progress;
}

void ScrollBar::set_scrollbar_length(double length) {
	/*if (height > m_height - SCROLL_WIDTH*2) {
		height = m_height - SCROLL_WIDTH*2;
	} else*/ if (length < SCROLL_WIDTH*0.5) {
		length = SCROLL_WIDTH*0.5;
	}
	m_scrollbar_height = length;
	m_bg.set_row_height(2, m_scrollbar_height);
	set_scroll_progress(m_progress);
}

void ScrollBar::set_section_color(ScrollWidget section, Color color) {
	switch (section) {
	case TOP_BUTTON:
		m_bg.set_cell_color(0, color);
		break;
	case TOP_TRACK:
		m_bg.set_cell_color(1, color);
		break;
	case TRACKER:
		m_bg.set_cell_color(2, color);
		break;
	case BOTTOM_TRACK:
		m_bg.set_cell_color(3, color);
		break;
	case BOTTOM_BUTTON:
		m_bg.set_cell_color(4, color);
		break;
	case BUTTONS:
		m_bg.set_cell_color(0, color);
		m_bg.set_cell_color(4, color);
		break;
	case TRACK:
		m_bg.set_cell_color(1, color);
		m_bg.set_cell_color(3, color);
		break;
	default: break;
	}
}

void ScrollBar::set_border_color(Color color) {
	m_bg.set_border_color(color);
}

void ScrollBar::set_border_width(double width) {
	m_bg.set_border_width(width);
}

void ScrollBar::set_scroll_speed(double speed) {
	m_scroll_speed = fabs(speed);
}

void ScrollBar::set_track_speed(double speed) {
	m_track_speed = fabs(speed);
}

double ScrollBar::get_scroll_speed() const {
	return m_scroll_speed;
}

double ScrollBar::get_track_speed() const {
	return m_track_speed;
}

void ScrollBar::set_horizontal(bool horiz) {
	m_horizontal = horiz;
	// We need to break the link, or else bad stuff happens
	m_linked = NULL;
	if (horiz) {
		m_bg.set_rotation(-90);
	} else {
		m_bg.set_rotation(0);
	}
}

void ScrollBar::relink(ScrollArea* linked) {
	if (m_updated) {
		return;
	}
	m_linked = linked;
	if (linked != NULL) {
		m_updated = true;
		if (m_horizontal) {
			linked->horiz_relink(this);
			linked->set_horiz_scroll_progress(m_progress);
		} else {
			linked->vert_relink(this);
			linked->set_vert_scroll_progress(m_progress);
		}
		m_updated = false;
	}
}

ScrollArea* ScrollBar::get_linked() {
	return m_linked;
}

void ScrollBar::draw(const GameWindow* window) const {
	if (!m_bg.is_invisible()) {
		glPushMatrix();
		transform_gl();
		m_bg.draw(window);
		glPopMatrix();
	}
}
