/*
 * client/ScrollArea.cpp
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

#include "ScrollArea.hpp"
#include "ScrollBar.hpp"
#include <cmath>

using namespace LM;
using namespace std;

ScrollArea::ScrollArea(double width, double height, double content_width, double content_height, ScrollBar* hbar, ScrollBar* vbar) {
	m_horiz_progress = 0.0;
	m_vert_progress = 0.0;
	m_updated = false;
	if (hbar != NULL) {
		hbar->set_horizontal(true);
		hbar->relink(this);
	} else {
		m_horiz_linked = NULL;
	}
	if (vbar != NULL) {
		vbar->set_horizontal(false);
		vbar->relink(this);
	} else {
		m_vert_linked = NULL;
	}
	m_width = width;
	m_height = height;
	set_content_height(content_height);
	set_content_width(content_width);
}

ScrollArea::ScrollArea(const ScrollArea& other) : m_group(other.m_group) {
	m_horiz_progress = other.m_horiz_progress;
	m_vert_progress = other.m_vert_progress;
	m_updated = false;
	m_horiz_linked = NULL;
	m_vert_linked = NULL;
	m_width = other.m_width;
	m_height = other.m_height;
	m_content_width = other.m_content_width;
	m_content_height = other.m_content_height;
}

ScrollArea* ScrollArea::clone() const {
	return new ScrollArea(*this);
}

void ScrollArea::set_width(double width) {
	m_width = width;
}

void ScrollArea::set_height(double height) {
	m_height = height;
}

void ScrollArea::set_content_width(double width) {
	double progress_pixels = m_horiz_progress*(m_content_width-m_width);
	m_content_width = width;
	if (m_horiz_linked != NULL) {
		double scrollbar_length = m_width/m_content_width;
		if (scrollbar_length > 1) {
			scrollbar_length = 1;
		}
		scrollbar_length *= m_horiz_linked->get_length() - ScrollBar::SCROLL_WIDTH*2;
		m_horiz_linked->set_scrollbar_length(scrollbar_length);
	}
	set_horiz_scroll_progress_pixels(progress_pixels);
}

void ScrollArea::set_content_height(double height) {
	double progress_pixels = m_vert_progress*(m_content_height-m_height);
	m_content_height = height;
	if (m_vert_linked != NULL) {
		double scrollbar_length = m_height/m_content_height;
		if (scrollbar_length > 1) {
			scrollbar_length = 1;
		}
		scrollbar_length *= m_vert_linked->get_length() - ScrollBar::SCROLL_WIDTH*2;
		m_vert_linked->set_scrollbar_length(scrollbar_length);
	}
	set_vert_scroll_progress_pixels(progress_pixels);
}

double ScrollArea::get_width() const {
	return m_width;
}

double ScrollArea::get_height() const {
	return m_height;
}

double ScrollArea::get_content_height() const {
	return m_content_height;
}

double ScrollArea::get_content_width() const {
	return m_content_width;
}

void ScrollArea::set_horiz_scroll_progress(double amount) {
	if (amount < 0) {
		amount = 0;
	} else if (amount > 1) {
		amount = 1;
	}

	m_horiz_progress = amount;

	if(m_horiz_linked != NULL && !m_updated) {
		m_updated = true;
		m_horiz_linked->set_scroll_progress(amount);
		m_updated = false;
	}
}

void ScrollArea::horiz_scroll(double amount) {
	set_horiz_scroll_progress(m_horiz_progress + amount);
}

void ScrollArea::set_horiz_scroll_progress_pixels(double pixels) {
	if (m_content_width <= m_width) {
		set_horiz_scroll_progress(0);
	} else {
		set_horiz_scroll_progress(pixels/(m_content_width-m_width));
	}
}

void ScrollArea::horiz_scroll_pixels(double pixels) {
	if (m_content_width > m_width) {
		horiz_scroll(pixels/(m_content_width-m_width));
	}
}

double ScrollArea::get_horiz_scroll_progress() const {
	return m_horiz_progress;
}

double ScrollArea::get_horiz_scroll_progress_pixels() const {
	return max<double>(0,m_horiz_progress*(m_content_width-m_width));
}

void ScrollArea::set_vert_scroll_progress(double amount) {
	if (amount < 0) {
		amount = 0;
	} else if (amount > 1) {
		amount = 1;
	}

	m_vert_progress = amount;

	if(m_vert_linked != NULL && !m_updated) {
		m_updated = true;
		m_vert_linked->set_scroll_progress(amount);
		m_updated = false;
	}
}

void ScrollArea::vert_scroll(double amount) {
	set_vert_scroll_progress(m_vert_progress + amount);
}

void ScrollArea::set_vert_scroll_progress_pixels(double pixels) {
	if (m_content_height <= m_height) {
		set_vert_scroll_progress(0);
	} else {
		set_vert_scroll_progress(pixels/(m_content_height-m_height));
	}
}

void ScrollArea::vert_scroll_pixels(double pixels) {
	if (m_content_height > m_height) {
		horiz_scroll(pixels/(m_content_height-m_height));
	}
}

double ScrollArea::get_vert_scroll_progress() const {
	return m_vert_progress;
}

double ScrollArea::get_vert_scroll_progress_pixels() const {
	return max<double>(0,m_vert_progress*(m_content_height-m_height));
}

GraphicGroup* ScrollArea::get_group() {
	return &m_group;
}

void ScrollArea::horiz_relink(ScrollBar* linked) {
	if (m_updated) {
		return;
	}
	m_horiz_linked = linked;
	if (linked != NULL) {
		m_updated = true;
		linked->relink(this);
		linked->set_scroll_progress(m_horiz_progress);
		m_updated = false;
	}
}

ScrollBar* ScrollArea::get_horiz_linked() {
	return m_horiz_linked;
}

void ScrollArea::vert_relink(ScrollBar* linked) {
	if (m_updated) {
		return;
	}
	m_vert_linked = linked;
	if (linked != NULL) {
		m_updated = true;
		linked->relink(this);
		linked->set_scroll_progress(m_vert_progress);
		m_updated = false;
	}
}

ScrollBar* ScrollArea::get_vert_linked() {
	return m_vert_linked;
}

void ScrollArea::draw(const GameWindow* window) const {
	if (!m_group.is_invisible()) {
		glPushMatrix();
		transform_gl();
		glEnable(GL_STENCIL_TEST);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisable(GL_TEXTURE_2D);
		glClearStencil(0);
		glClear(GL_STENCIL_BUFFER_BIT);
		glStencilFunc(GL_ALWAYS, 1, 1);
		glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
		glColor4d(1.0, 1.0, 1.0, 1.0);
		draw_rect(0.0, 0.0, m_width, m_height);
		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		glStencilFunc(GL_EQUAL, 1, 1);
		glTranslated(-round(m_horiz_progress*(m_content_width-m_width)), -round(m_vert_progress*(m_content_height-m_height)), 0);
		glEnable(GL_TEXTURE_2D);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		m_group.draw(window);
		glDisable(GL_STENCIL_TEST);
		glPopMatrix();
	}
}
