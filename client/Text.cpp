/*
 * client/Text.cpp
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

#include "Text.hpp"
#include "common/Exception.hpp"
#include "compat_gl.h"

using namespace std;

Text::Text(const string& text, Font* font) : Graphic() {
	m_fg = font->render_string(text);
	if (m_fg == NULL) {
		throw LMException("Cannot render text");
	}
	m_shadow = NULL;
	m_fg->set_center_x(0);
	m_fg->set_center_y(0);
	m_image_width = m_fg->get_image_width();
	m_image_height = m_fg->get_image_height();
	m_shadow_enabled = false;
}

Text::Text(const Text& other) : Graphic(other) {
	m_fg = other.m_fg->clone();
	if (other.m_shadow != NULL) {
		m_shadow = other.m_shadow->clone();
	}
	m_image_width = m_fg->get_image_width();
	m_image_height = m_fg->get_image_height();
	m_shadow_enabled = other.m_shadow_enabled;
}

Text::~Text() {
	delete m_fg;
	delete m_shadow;
}

Text* Text::clone() const {
	return new Text(*this);
}

void Text::touch_shadow() {
	if (m_shadow == NULL) {
		m_shadow = new Sprite(*m_fg);
	}
}

void Text::set_color(double r, double g, double b) {
	m_fg->set_red_intensity(r);
	m_fg->set_green_intensity(g);
	m_fg->set_blue_intensity(b);
}

void Text::set_color(const Color& color) {
	set_color(color.r, color.g, color.b);
}

void Text::set_alpha(double a) {
	m_fg->set_alpha(a);
}

void Text::set_shadow_color(double r, double g, double b) {
	touch_shadow();
	m_shadow->set_red_intensity(r);
	m_shadow->set_green_intensity(g);
	m_shadow->set_blue_intensity(b);
}

void Text::set_shadow_color(const Color& color) {
	set_shadow_color(color.r, color.g, color.b);
}

void Text::set_shadow_alpha(double a) {
	touch_shadow();
	m_shadow->set_alpha(a);
}

void Text::set_shadow_offset(double x, double y) {
	touch_shadow();
	m_shadow->set_x(x);
	m_shadow->set_y(y);
}

void Text::set_shadow(bool enable) {
	m_shadow_enabled = enable;
	if (enable) {
		touch_shadow();
	}
}

void Text::set_red_intensity(double r) {
	m_fg->set_red_intensity(r);
}

void Text::set_green_intensity(double g) {
	m_fg->set_green_intensity(g);
}

void Text::set_blue_intensity(double b) {
	m_fg->set_blue_intensity(b);
}

void Text::draw(const GameWindow* window) const {
	glPushMatrix();
	transform_gl();
	GLfloat vec[4] = { 0.0, 0.0, 0.0, 0.8 };
	glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_COMBINE);
	glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_ALPHA,GL_INTERPOLATE);
	glTexEnvfv(GL_TEXTURE_ENV,GL_TEXTURE_ENV_COLOR,vec);
	glEnable(GL_ALPHA_TEST);
	if (m_shadow_enabled) {
		glAlphaFunc(GL_GREATER,m_shadow->get_alpha()*0.2);
		m_shadow->draw(window);
	}
	glAlphaFunc(GL_GREATER,m_fg->get_alpha()*0.2);
	m_fg->draw(window);
	glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
	glDisable(GL_ALPHA_TEST);
	glPopMatrix();
}
