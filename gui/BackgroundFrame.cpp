/*
 * gui/BackgroundFrame.cpp
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

#include "BackgroundFrame.hpp"
#include "DrawContext.hpp"

using namespace LM;
using namespace std;

BackgroundFrame::BackgroundFrame(Widget* parent) : Widget(parent) {
	m_border_radius = 0;
	m_border_padding = 0;
	m_max_corner_sections = 16;
	m_use_thin_border = false;
}

void BackgroundFrame::set_border_radius(float radius) {
	if (radius < 0) {
		radius = 0;
	}
	
	m_border_radius = radius;
}

float BackgroundFrame::get_border_radius() const {
	return m_border_radius;
}

void BackgroundFrame::set_border_padding(float padding) {
	if (padding < 0) {
		padding = 0;
	}
	
	m_border_padding = padding;
}

float BackgroundFrame::get_border_padding() const {
	return m_border_padding;
}

void BackgroundFrame::set_border_max_roundness(int sections) {
	if (sections < 1) {
		sections = 1;
	}
	
	m_max_corner_sections = sections;
}

void BackgroundFrame::set_thin_border(bool thin) {
	m_use_thin_border = thin;
}

float BackgroundFrame::get_internal_width() const {
	return get_width() - 2 * m_border_radius - 2 * m_border_padding;
}

float BackgroundFrame::get_internal_height() const {
	return get_height() - 2 * m_border_radius - 2 * m_border_padding;
}

void BackgroundFrame::draw(DrawContext* ctx) const {
	ctx->push_transform();
	ctx->translate(get_x(), get_y());
	ctx->set_draw_color(get_color(COLOR_SECONDARY));
	if (m_use_thin_border) {
		ctx->draw_roundrect_line(get_width(), get_height(), m_border_radius, m_max_corner_sections);
	} else {
		ctx->draw_roundrect_wide_line(get_width(), get_height(), m_border_radius, m_max_corner_sections);
	}
	ctx->set_draw_color(get_color(COLOR_PRIMARY));
	if (m_use_thin_border) {
		ctx->draw_roundrect_fill(get_width() - m_border_padding*2, get_height() - m_border_padding*2, m_border_radius, m_max_corner_sections);
	} else {
		ctx->draw_rect_fill(get_width() - m_border_radius*2 - m_border_padding*2, get_height() - m_border_radius*2 - m_border_padding*2);
	}
	ctx->pop_transform();
	
	setup_clip(ctx);
	
	draw_internals(ctx);
	
	draw_extras(ctx);

	reset_clip(ctx);
}

void BackgroundFrame::draw_internals(DrawContext* ctx) const {
	//setup_clip(ctx);
	
	Widget::draw_internals(ctx);
	
	//reset_clip(ctx);
}

void BackgroundFrame::setup_clip(DrawContext* ctx) const {
	// Set up clip areas
	ctx->push_clip();
	ctx->push_transform();
	ctx->set_draw_color(Color(1.0f, 1.0f, 1.0f, 1.0f));
	ctx->translate(ctx->get_width()/2 - get_absolute_x(), ctx->get_height()/2 - get_absolute_y());
	ctx->start_clip();
	// XXX: Would be better to have a canonical way to move exactly back to the base screen projection, so we don't
	// have to draw a huge rectangle here.
	ctx->draw_rect_fill(ctx->get_width() * 100.0, ctx->get_height() * 100.0);
	ctx->finish_clip();
	ctx->pop_transform();
	
	ctx->push_transform();
	ctx->translate(get_x(), get_y());
	ctx->start_clip();
	ctx->clip_sub();
	ctx->draw_rect_fill(get_width() - 2 * m_border_radius - 2 * m_border_padding, get_height() - 2 * m_border_radius - 2 * m_border_padding);
	ctx->finish_clip();
	ctx->pop_transform();
	ctx->pop_clip();
}

void BackgroundFrame::reset_clip(DrawContext* ctx) const {
	// Reset clip areas to previous state
	ctx->push_clip();
	ctx->set_draw_color(Color(1.0f, 1.0f, 1.0f, 1.0f));
	ctx->push_transform();
	ctx->translate(get_x(), get_y());
	ctx->start_clip();
	ctx->draw_rect_fill(get_width() - 2 * m_border_radius - 2 * m_border_padding, get_height() - 2 * m_border_radius - 2 * m_border_padding);
	ctx->finish_clip();
	ctx->pop_transform();

	ctx->push_transform();
	ctx->translate(ctx->get_width()/2 - get_absolute_x(), ctx->get_height()/2 - get_absolute_y());
	ctx->start_clip();
	ctx->clip_sub();
	ctx->draw_rect_fill(ctx->get_width() * 100.0, ctx->get_height() * 100.0);
	ctx->finish_clip();
	ctx->pop_transform();
	ctx->pop_clip();
}

void BackgroundFrame::draw_extras(DrawContext* ctx) const {
}
