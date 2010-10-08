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

using namespace LM;
using namespace std;

Graphic::Graphic(Image* image) : m_image(*image), m_color(Color::WHITE) {
	m_invisible = false;
	m_shaders = NULL;

	if (m_image.get_handle() == 0) {
		m_image.gen_handle(true);
	}

}

Graphic::Graphic(const Graphic& other) : m_image(other.m_image), m_color(other.m_color) {
	m_bone = other.m_bone;
	m_invisible = other.m_invisible;
}

Graphic::~Graphic() {
	// Nothing to do
}

const Image* Graphic::get_image() const {
	return &m_image;
}

void Graphic::transform(DrawContext* ctx) const {
	m_bone.transform(ctx);
}

void Graphic::preprocess(DrawContext* ctx) const {
	ctx->set_draw_color(m_color);
	if (m_shaders != NULL) {
		ctx->bind_shader_set(m_shaders);
	}
}

void Graphic::postprocess(DrawContext* ctx) const {
	ctx->unbind_shader_set();
}

void Graphic::set_x(float x) {
	m_bone.set_x(x);
}

void Graphic::set_y(float y) {
	m_bone.set_y(y);
}

void Graphic::set_scale_x(float scale_x) {
	m_bone.set_scale_x(scale_x);
}

void Graphic::set_scale_y(float scale_y) {
	m_bone.set_scale_y(scale_y);
}

void Graphic::set_rotation(float rotation) {
	m_bone.set_rotation(rotation);
}

float Graphic::get_x() const {
	return m_bone.get_x();
}

float Graphic::get_y() const {
	return m_bone.get_y();
}

float Graphic::get_scale_x() const {
	return m_bone.get_scale_x();
}

float Graphic::get_scale_y() const {
	return m_bone.get_scale_y();
}

float Graphic::get_rotation() const {
	return m_bone.get_rotation();
}

void Graphic::set_center_x(float center_x) {
	m_bone.set_center_x(center_x);
}

void Graphic::set_center_y(float center_y) {
	m_bone.set_center_y(center_y);
}

float Graphic::get_center_x() const {
	return m_bone.get_center_x();
}

float Graphic::get_center_y() const {
	return m_bone.get_center_y();
}

void Graphic::set_bone(const Bone& bone) {
	m_bone = bone;
}

Bone* Graphic::get_bone() {
	return &m_bone;
}

void Graphic::set_invisible(bool invisible) {
	m_invisible = invisible;
}

bool Graphic::is_invisible() const {
	return m_invisible;
}

void Graphic::set_color(const Color& c) {
	m_color = c;
}

const Color& Graphic::get_color() const {
	return m_color;
}

void Graphic::set_shader_set(ShaderSet* shaders) {
	m_shaders = shaders;
}
