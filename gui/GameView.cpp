/*
 * gui/GameView.cpp
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

#include "GameView.hpp"
#include "GLESContext.hpp"
#include "GraphicRegion.hpp"

using namespace LM;
using namespace std;

GameView::GameView(const string& name, ResourceCache* cache, int width, int height, int overscan, Widget* parent) : Widget(parent) {
	m_offset_x = 0;
	m_offset_y = 0;
	m_scale_base = 1536;

	// TODO find a way to pass down the context type
	m_ctx = new GLESContext(width + 2*overscan, height + 2*overscan, true);
	m_overscan = overscan;
	Image img = m_ctx->get_image(name, cache);
	m_ctxi = new GraphicRegion(&img);
	m_ctxi->set_image_x(-overscan);
	m_ctxi->set_image_y(-overscan);
	m_ctxi->set_width(width);
	m_ctxi->set_height(height);
	m_ctxi->set_scale_y(-1.0f);
	m_ctxi->set_y(m_ctxi->get_height());

	set_width(width);
	set_height(height);
}

void GameView::recalc_scale() {
	// TODO put base scale elsewhere
	m_scale = max<float>(get_width(), get_height()) / m_scale_base;
}

void GameView::set_width(float w) {
	Widget::set_width(w);
	recalc_scale();
}

void GameView::set_height(float h) {
	Widget::set_height(h);
	recalc_scale();
}

void GameView::set_offset_x(float x) {
	m_offset_x = x;
}

void GameView::set_offset_y(float y) {
	m_offset_y = y;
}

void GameView::set_scale_base(float s) {
	m_scale_base = s;
}

float GameView::get_scale() const {
	return m_scale;
}

Point GameView::world_to_view(Point world) const {
	return (world + Point(get_width()/2 - m_offset_x, get_height()/2 - m_offset_y))*m_scale;
}

void GameView::draw(DrawContext* ctx) const {
	m_ctx->make_active();
	m_ctx->clear();
	m_ctx->set_active_camera();
	m_ctx->push_transform();
	m_ctx->translate(get_width()/2 - m_offset_x, get_height()/2 - m_offset_y);
	m_ctx->scale(m_scale, m_scale);
	m_ctx->set_active_graphics();
	m_ctx->push_transform();

	Widget::draw(m_ctx);

	m_ctx->set_active_camera();
	m_ctx->pop_transform();
	m_ctx->set_active_graphics();
	m_ctx->pop_transform();
	
	ctx->make_active();
	m_ctxi->draw(ctx);
}
