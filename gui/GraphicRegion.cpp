/*
 * gui/GraphicRegion.cpp
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

#include "GraphicRegion.hpp"
#include "DrawContext.hpp"

using namespace LM;
using namespace std;

GraphicRegion::GraphicRegion(Image* image) : Graphic(image) {
	m_img_x = 0;
	m_img_y = 0;
	m_img_width = image->get_width();
	m_img_height = image->get_height();

	m_width = m_img_width;
	m_height = m_img_height;

	m_repeat = true;
}

GraphicRegion::GraphicRegion(const GraphicRegion& other) : Graphic(other) {
	m_width = other.m_width;
	m_height = other.m_height;
	m_img_x = other.m_img_x;
	m_img_y = other.m_img_y;
	m_img_width = other.m_img_width;
	m_img_height = other.m_img_height;

	m_repeat = other.m_repeat;
}

GraphicRegion* GraphicRegion::clone() const {
	return new GraphicRegion(*this);
}

void GraphicRegion::set_width(int w) {
	m_width = w;
}

void GraphicRegion::set_height(int h) {
	m_height = h;
}

int GraphicRegion::get_width() const {
	return m_width;
}

int GraphicRegion::get_height() const {
	return m_height;
}

void GraphicRegion::set_image_x(float x) {
	m_img_x = x;
}

void GraphicRegion::set_image_y(float y) {
	m_img_y = y;
}

void GraphicRegion::set_image_width(float w) {
	m_img_width = w;
}

void GraphicRegion::set_image_height(float h) {
	m_img_height = h;
}

float GraphicRegion::get_image_x() const {
	return m_img_x;
}

float GraphicRegion::get_image_y() const {
	return m_img_y;
}

float GraphicRegion::get_image_width() const {
	return m_img_width;
}

float GraphicRegion::get_image_height() const {
	return m_img_height;
}

void GraphicRegion::set_image_repeat(bool repeat) {
	m_repeat = repeat;
}

void GraphicRegion::draw(DrawContext* ctx) const {
	ctx->push_transform();
	transform(ctx);
	preprocess(ctx);
	const Image& image = get_image();
	ctx->bind_image(image.get_handle());
	if (m_repeat) {
		ctx->draw_bound_image_tiled(m_width, m_height, m_img_x, m_img_y, m_img_width, m_img_height);
	} else {
		ctx->draw_bound_image_region(m_width, m_height, m_img_x, m_img_y, m_img_width, m_img_height);
	}
	ctx->unbind_image();
	postprocess(ctx);
	ctx->pop_transform();
}
