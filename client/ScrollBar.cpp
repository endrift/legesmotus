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

const double ScrollBar::DEFAULT_AUTOSCROLL = 50.0;

ScrollBar::ScrollBar(ScrollArea* area) : m_bg(5,SCROLL_WIDTH) {
	m_bg.set_corner_radius(SCROLL_WIDTH*0.5);
	m_bg.set_border_color(Color::WHITE);
	m_bg.set_border_width(2);
	m_bg.set_border_collapse(true);
	m_bg.set_row_height(0, SCROLL_WIDTH);
	m_bg.set_row_height(4, SCROLL_WIDTH);
	m_bg.set_center_x(SCROLL_WIDTH*0.5);

	m_updated = false;
	if (area != NULL) {
		area->relink(this);
	} else {
		m_linked = NULL;
	}
	m_progress = 0.0;
	set_height(SCROLL_WIDTH*5);
	set_scrollbar_height(SCROLL_WIDTH);
	m_window = NULL;
	m_pressed = NO_WIDGET;
}

void ScrollBar::autoscroll(double scale) {
	double amount = 0;
	switch (m_pressed) {
	case TOP_BUTTON:
		amount = -2;
		break;
	case TOP_TRACK:
		amount = -10;
		break;
	case BOTTOM_TRACK:
		amount = 10;
		break;
	case BOTTOM_BUTTON:
		amount = 2;
		break;
	default: break;
	}

	if (m_linked != NULL) {
		m_linked->scroll_pixels(amount*10*scale/m_height);
	} else {
		scroll(amount*scale/m_height);
	}
}

void ScrollBar::set_x(double x) {
	m_bg.set_x(x);
}

void ScrollBar::set_y(double y) {
	m_bg.set_y(y);
}

void ScrollBar::set_height(double height) {
	m_height = height;
	m_bg.set_center_y(height*0.5);
	set_scroll_progress(m_progress);
}

double ScrollBar::get_x() const {
	return m_bg.get_x();
}

double ScrollBar::get_y() const {
	return m_bg.get_y();
}

double ScrollBar::get_height() const {
	return m_height;
}

void ScrollBar::mouse_button_event(const SDL_MouseButtonEvent& event) {
	if (event.state == SDL_RELEASED) {
		m_pressed = NO_WIDGET;
		return;
	}

	if (event.x < (get_x() - SCROLL_WIDTH*0.5) || event.x > (get_x() + SCROLL_WIDTH*0.5)) {
		return;
	} else if(event.y < (get_y() - m_height*0.5) || event.y > (get_y() + m_height*0.5)) {
		return;
	}

	if(event.y < (get_y() - (m_height*0.5 - SCROLL_WIDTH))) {
		m_pressed = TOP_BUTTON;
		autoscroll(DEFAULT_AUTOSCROLL);
	} else if(event.y > (get_y() + (m_height*0.5 - SCROLL_WIDTH))) {
		m_pressed = BOTTOM_BUTTON;
		autoscroll(DEFAULT_AUTOSCROLL);
	} else if(event.y < (get_y() - (m_height*0.5 - SCROLL_WIDTH) +
			m_bg.get_row_height(1))) {
		m_pressed = TOP_TRACK;
		autoscroll(DEFAULT_AUTOSCROLL);
	} else if(event.y > (get_y() + (m_height*0.5 - SCROLL_WIDTH) -
			m_bg.get_row_height(3))) {
		m_pressed = BOTTOM_TRACK;
		autoscroll(DEFAULT_AUTOSCROLL);
	} else {
		m_pressed = TRACKER;
		m_grab_y = event.y - m_progress*(m_height-SCROLL_WIDTH*2-m_scrollbar_height);
	}
}

void ScrollBar::mouse_motion_event(const SDL_MouseMotionEvent& event) {
	if (m_pressed != TRACKER) {
		return;
	}
	set_scroll_progress((event.y - m_grab_y)/(m_height-SCROLL_WIDTH*2-m_scrollbar_height));
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
		m_linked->set_scroll_progress(amount);
		m_updated = false;
	}
}

void ScrollBar::scroll(double amount) {
	set_scroll_progress(m_progress + amount);
}

double ScrollBar::get_scroll_progress() const {
	return m_progress;
}

void ScrollBar::set_scrollbar_height(double height) {
	/*if (height > m_height - SCROLL_WIDTH*2) {
		height = m_height - SCROLL_WIDTH*2;
	} else*/ if (height < SCROLL_WIDTH*0.5) {
		height = SCROLL_WIDTH*0.5;
	}
	m_scrollbar_height = height;
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

void ScrollBar::register_window(GameWindow* window) {
	if(m_window != NULL) unregister_window();
	window->register_graphic(&m_bg);
	m_window = window;
}

void ScrollBar::unregister_window() {
	m_window->unregister_graphic(&m_bg);
}

void ScrollBar::relink(ScrollArea* linked) {
	if (m_updated) {
		return;
	}
	m_linked = linked;
	if (linked != NULL) {
		m_updated = true;
		linked->relink(this);
		linked->set_scroll_progress(m_progress);
		m_updated = false;
	}
}

ScrollArea* ScrollBar::getLinked() {
	return m_linked;
}
