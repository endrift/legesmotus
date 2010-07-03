/*
 * gui/Graphic.cpp
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

#include "Graphic.hpp"
#include <cmath>

using namespace LM;
using namespace std;

Graphic::Graphic(Image* image) : m_image(*image), m_color(Color::WHITE) {
	m_x = 0;
	m_y = 0;
	m_center_x = 0;
	m_center_y = 0;
	m_scale_x = 1;
	m_scale_y = 1;
	m_rotation = 0;

	m_invisible = false;

	if (m_image.get_handle() == 0) {
		m_image.gen_handle(true);
	}
}

Graphic::Graphic(const Graphic& other) : m_image(other.m_image), m_color(other.m_color) {
	m_x = other.m_x;
	m_y = other.m_y;
	m_center_x = other.m_center_y;
	m_scale_x = other.m_scale_x;
	m_scale_y = other.m_scale_y;
	m_rotation = other.m_rotation;
	m_invisible = other.m_invisible;
}

Graphic::~Graphic() {
	// Nothing to do
}

const Image* Graphic::get_image() const {
	return &m_image;
}

void Graphic::transform(DrawContext* ctx) const {
	ctx->translate(round(m_x), round(m_y));
	ctx->rotate(m_rotation);
	ctx->scale(m_scale_x, m_scale_y);
	ctx->translate(-round(m_center_x), -round(m_center_y));
}

void Graphic::set_x(float x) {
	m_x = x;
}

void Graphic::set_y(float y) {
	m_y = y;
}

void Graphic::set_scale_x(float scale_x) {
	m_scale_x = scale_x;
}

void Graphic::set_scale_y(float scale_y) {
	m_scale_y = scale_y;
}

void Graphic::set_rotation(float rotation) {
	m_rotation = rotation;
}

float Graphic::get_x() const {
	return m_x;
}

float Graphic::get_y() const {
	return m_y;
}

float Graphic::get_scale_x() const {
	return m_scale_x;
}

float Graphic::get_scale_y() const {
	return m_scale_y;
}

float Graphic::get_rotation() const {
	return m_rotation;
}

void Graphic::set_center_x(float center_x) {
	m_center_x = center_x;
}

void Graphic::set_center_y(float center_y) {
	m_center_y = center_y;
}

float Graphic::get_center_x() const {
	return m_center_x;
}

float Graphic::get_center_y() const {
	return m_center_y;
}
void Graphic::set_invisible(bool invisible) {
	m_invisible = invisible;
}

bool Graphic::is_invisible() const {
	return m_invisible;
}
