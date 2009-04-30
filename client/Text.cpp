/*
 * client/Text.cpp
 *
 * Copyright 2009 - Nathan Partlan, Andrew Ayer, Daniel Schneider, and Jeffrey Pfau
 * 
 */

#include "Text.hpp"
#include "common/LMException.hpp"

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

Text::~Text() {
	delete m_fg;
	delete m_shadow;
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

void Text::set_alpha(double a) {
	m_fg->set_alpha(a);
}

void Text::set_shadow_color(double r, double g, double b) {
	touch_shadow();
	m_shadow->set_red_intensity(r);
	m_shadow->set_green_intensity(g);
	m_shadow->set_blue_intensity(b);
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
	if (m_shadow_enabled) {
		m_shadow->draw(window);
	}
	m_fg->draw(window);
	glPopMatrix();
}
