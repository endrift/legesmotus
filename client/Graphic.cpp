/*
 * client/Graphic.cpp
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
#include "SDL_image.h"
#include "common/Exception.hpp"
#include "common/math.hpp"

using namespace LM;

Graphic::Graphic() {
	init(NULL);
}

Graphic::Graphic(SDL_Surface* image) {
	init(image);
}

Graphic::Graphic(const char* filename) {
	SDL_Surface* loaded = IMG_Load(filename);
	init(loaded);
	SDL_FreeSurface(loaded);
}

Graphic::Graphic(const Graphic& other) {
	m_x = other.m_x;
	m_y = other.m_y;
	m_invisible = other.m_invisible;
	m_center_x = other.m_center_x;
	m_center_y = other.m_center_y;
	m_rotation = other.m_rotation;
	m_scale_x = other.m_scale_x;
	m_scale_y = other.m_scale_y;
	m_priority = other.m_priority;
	m_image_width = other.m_image_width;
	m_image_height = other.m_image_height;
	m_tex_id = other.m_tex_id;
	m_tex_count = other.m_tex_count;
	if (m_tex_count != NULL) {
		++*m_tex_count;
	}
}

Graphic::~Graphic() {
	if (m_tex_count != NULL) {
		if(*m_tex_count <= 1) {
			glDeleteTextures(1,&m_tex_id);
			delete m_tex_count;
		} else {
			--*m_tex_count;
		}
	}
}

void Graphic::init(SDL_Surface* image) {
	m_image_width = 0;
	m_image_height = 0;
	m_center_x = 0.0;
	m_center_y = 0.0;
	m_invisible = false;
	m_tex_count = NULL;
	m_priority = 0;
	m_x = 0;
	m_y = 0;
	m_scale_x = 1.0;
	m_scale_y = 1.0;
	m_rotation = 0;

	if (image == NULL) {
		return;
	}

	m_image_width = image->w;
	m_image_height = image->h;
	int width = to_pow_2(image->w);
	int height = to_pow_2(image->h);
	SDL_Surface *hw_image = SDL_CreateRGBSurface(SDL_SWSURFACE, width, height, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
	SDL_SetAlpha(image, 0, SDL_ALPHA_OPAQUE);
	SDL_BlitSurface(image, NULL, hw_image, NULL);
	m_tex_count = new int;
	*m_tex_count = 1;
	glGenTextures(1, &m_tex_id);
	glBindTexture(GL_TEXTURE_2D, m_tex_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, 4, width, height, 0, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, hw_image->pixels);
	SDL_FreeSurface(hw_image);
}

GLuint Graphic::get_texture_id() const {
	return m_tex_id;
}

void Graphic::transform_gl() const {
	glTranslated(round(m_x), round(m_y), 0.0); //TODO find alternative method
	glRotated(m_rotation, 0.0, 0.0, 1.0);
	glScaled(m_scale_x, m_scale_y, 1.0);
	glTranslated(-round(m_center_x), -round(m_center_y), 0.0);
}

void Graphic::draw_rect(double x0, double y0, double x1, double y1) const {
	GLdouble vertices[8] = {
		x0, y0,
		x1, y0,
		x1, y1,
		x0, y1
	};
	glVertexPointer(2, GL_DOUBLE, 0, vertices);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

double Graphic::get_image_width() const {
	return m_image_width;
}

double Graphic::get_image_height() const {
	return m_image_height;
}

double Graphic::get_x() const {
	return m_x;
}

double Graphic::get_y() const {
	return m_y;
}

double Graphic::get_scale_x() const {
	return m_scale_x;
}

double Graphic::get_scale_y() const {
	return m_scale_y;
}

double Graphic::get_rotation() const {
	return m_rotation;
}

int Graphic::get_priority() const {
	return m_priority;
}

void Graphic::set_priority(int priority) {
	m_priority = priority;
}

void Graphic::set_x(double x) {
	m_x = x;
}

void Graphic::set_y(double y) {
	m_y = y;
}

void Graphic::set_scale_x(double scale_x) {
	m_scale_x = scale_x;
}

void Graphic::set_scale_y(double scale_y) {
	m_scale_y = scale_y;
}

void Graphic::set_rotation(double rotation) {
	m_rotation = rotation;
}

double Graphic::get_center_x() const {
	return m_center_x;
}

double Graphic::get_center_y() const {
	return m_center_y;
}

void Graphic::set_center_x(double center_x) {
	m_center_x = center_x;
}

void Graphic::set_center_y(double center_y) {
	m_center_y = center_y;
}

void Graphic::set_color_intensity(const Color& color) {
	set_red_intensity(color.r);
	set_red_intensity(color.g);
	set_red_intensity(color.b);
}

bool Graphic::is_invisible() const {
	return m_invisible;
}

void Graphic::set_invisible(bool invisible) {
	m_invisible = invisible;
}

bool Graphic::is_over(int x, int y) const {
	double left = get_x() - get_center_x();
	double top = get_y() - get_center_y();
	return (x >= left && x <= left + get_image_width() && y >= top && y <= top + get_image_height());
}
