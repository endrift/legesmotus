/*
 * gui/Bone.cpp
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

#include "Bone.hpp"
#include "DrawContext.hpp"

#include <cmath>

using namespace LM;
using namespace std;

Bone::Bone(Bone* parent) {
	m_x = 0;
	m_y = 0;
	m_center_x = 0;
	m_center_y = 0;
	m_scale_x = 1;
	m_scale_y = 1;
	m_rotation = 0;

	m_parent = NULL;
}

void Bone::set_parent(Bone* parent) {
	m_parent = parent;
}

void Bone::set_x(float x) {
	m_x = x;
}

void Bone::set_y(float y) {
	m_y = y;
}

void Bone::set_scale_x(float scale_x) {
	m_scale_x = scale_x;
}

void Bone::set_scale_y(float scale_y) {
	m_scale_y = scale_y;
}

void Bone::set_rotation(float rotation) {
	m_rotation = rotation;
}

float Bone::get_x() const {
	return m_x;
}

float Bone::get_y() const {
	return m_y;
}

float Bone::get_scale_x() const {
	return m_scale_x;
}

float Bone::get_scale_y() const {
	return m_scale_y;
}

float Bone::get_rotation() const {
	return m_rotation;
}

void Bone::set_center_x(float center_x) {
	m_center_x = center_x;
}

void Bone::set_center_y(float center_y) {
	m_center_y = center_y;
}

float Bone::get_center_x() const {
	return m_center_x;
}

float Bone::get_center_y() const {
	return m_center_y;
}

void Bone::transform(DrawContext* ctx) const {
	if (m_parent != NULL) {
		m_parent->transform(ctx);
	}
	ctx->translate(round(m_x), round(m_y));
	ctx->rotate(m_rotation);
	ctx->scale(m_scale_x, m_scale_y);
	ctx->translate(-round(m_center_x), -round(m_center_y));
}

void Bone::diagnostic_draw(DrawContext* ctx) const {
	ctx->push_transform();
	if (m_parent != NULL) {
		m_parent->transform(ctx);
	}
	ctx->unbind_image();
	ctx->set_draw_color(Color(1.0, 0, 0, 0.5));
	ctx->draw_arc_line(1, 4, 4, 16);
	ctx->draw_line(0, 0, round(m_x), round(m_y));
	ctx->translate(round(m_x), round(m_y));
	ctx->rotate(m_rotation);
	ctx->scale(m_scale_x, m_scale_y);
	ctx->draw_line(0, 0, -round(m_center_x), -round(m_center_y));
	ctx->draw_arc_line(1, 8, 8, 16);
	ctx->draw_line(0, 0, 12, 0);
	ctx->translate(-round(m_center_x), -round(m_center_y));
	ctx->draw_rect_line(4, 4);
	ctx->pop_transform();
	ctx->set_draw_color(Color(1.0, 1.0, 1.0, 1.0));
}
