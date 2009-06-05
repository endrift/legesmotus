/*
 * client/ScrollArea.cpp
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

#include "ScrollArea.hpp"
#include "ScrollBar.hpp"
#include <cmath>

using namespace std;

ScrollArea::ScrollArea(double width, double height, double content_height, ScrollBar* bar) {
	m_progress = 0.0;
	m_updated = false;
	if (bar != NULL) {
		bar->relink(this);
	} else {
		m_linked = NULL;
	}
	m_width = width;
	m_height = height;
	set_content_height(content_height);
}

ScrollArea::ScrollArea(const ScrollArea& other) : m_group(other.m_group) {
	m_progress = other.m_progress;
	m_updated = false;
	m_linked = NULL;
	m_width = other.m_width;
	m_height = other.m_height;
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

void ScrollArea::set_content_height(double height) {
	double progress_pixels = m_progress*(m_content_height-m_height);
	m_content_height = height;
	if (m_linked != NULL) {
		double scrollbar_height = m_height/m_content_height;
		if (scrollbar_height > 1) {
			scrollbar_height = 1;
		}
		scrollbar_height *= m_linked->get_height() - ScrollBar::SCROLL_WIDTH*2;
		m_linked->set_scrollbar_height(scrollbar_height);
	}
	set_scroll_progress_pixels(progress_pixels);
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

void ScrollArea::set_scroll_progress(double amount) {
	if (amount < 0) {
		amount = 0;
	} else if (amount > 1) {
		amount = 1;
	}

	m_progress = amount;

	if(m_linked != NULL && !m_updated) {
		m_updated = true;
		m_linked->set_scroll_progress(amount);
		m_updated = false;
	}
}

void ScrollArea::scroll(double amount) {
	set_scroll_progress(m_progress + amount);
}

void ScrollArea::set_scroll_progress_pixels(double pixels) {
	if (m_content_height <= m_height) {
		set_scroll_progress(0);
	} else {
		set_scroll_progress(pixels/(m_content_height-m_height));
	}
}

void ScrollArea::scroll_pixels(double pixels) {
	if (m_content_height > m_height) {
		scroll(pixels/(m_content_height-m_height));
	}
}

double ScrollArea::get_scroll_progress() const {
	return m_progress;
}

double ScrollArea::get_scroll_progress_pixels() const {
	return max<double>(0,m_progress*(m_content_height-m_height));
}

GraphicGroup* ScrollArea::get_group() {
	return &m_group;
}

void ScrollArea::relink(ScrollBar* linked) {
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

ScrollBar* ScrollArea::getLinked() {
	return m_linked;
}

void ScrollArea::draw(const GameWindow* window) const {
	if (!m_group.is_invisible()) {
		glPushMatrix();
		transform_gl();
		glEnable(GL_STENCIL_TEST);
		glClearStencil(0);
		glClear(GL_STENCIL_BUFFER_BIT);
		glStencilFunc(GL_ALWAYS, 1, 1);
		glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
		glColor4d(1.0, 1.0, 1.0, 1.0);
		glBegin(GL_QUADS);
		glVertex2i(0,0);
		glVertex2i(int(m_width), 0);
		glVertex2i(int(m_width), int(m_height));
		glVertex2i(0, int(m_height));
		glEnd();
		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		glStencilFunc(GL_EQUAL, 1, 1);
		glTranslated(0,-round(m_progress*(m_content_height-m_height)),0);
		m_group.draw(window);
		glDisable(GL_STENCIL_TEST);
		glPopMatrix();
	}
}
