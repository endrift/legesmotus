/*
 * client/Sprite.cpp
 *
 * Copyright 2009 - Nathan Partlan, Andrew Ayer, Daniel Schneider, and Jeffrey Pfau
 * 
 */

#include "Sprite.hpp"
#include "GameWindow.hpp"
#include "common/math.hpp"

Sprite::Sprite(SDL_Surface* image) : Graphic(image) {
	init();
}

Sprite::Sprite(const char* filename) : Graphic(filename) {	
	init();
}

Sprite::Sprite(const Sprite& other) : Graphic(other) {
	m_alpha = other.m_alpha;
	m_red = other.m_red;
	m_green = other.m_green;
	m_blue = other.m_blue;
	m_width = other.m_width;
	m_height = other.m_height;
}

void Sprite::init() {
	m_alpha = 1.0;
	m_red = 1.0;
	m_green = 1.0;
	m_blue = 1.0;
	m_width = toPow2(get_image_width());
	m_height = toPow2(get_image_height());
	set_center_x(get_image_width()/2.0);
	set_center_y(get_image_height()/2.0);
	set_antialiasing(true);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

}

int Sprite::get_width() const {
	return m_width;
}

int Sprite::get_height() const {
	return m_height;
}

double Sprite::get_alpha() const {
	return m_alpha;
}

double Sprite::get_red_intensity() const {
	return m_red;
}

double Sprite::get_green_intensity() const {
	return m_green;
}

double Sprite::get_blue_intensity() const {
	return m_blue;
}

void Sprite::set_alpha(double alpha) {
	m_alpha = alpha;
}

void Sprite::set_red_intensity(double red) {
	m_red = red;
}

void Sprite::set_green_intensity(double green) {
	m_green = green;
}

void Sprite::set_blue_intensity(double blue) {
	m_blue = blue;
}

void Sprite::set_antialiasing(bool enable) {
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, enable?GL_LINEAR:GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, enable?GL_LINEAR:GL_NEAREST);
}

void Sprite::draw(const GameWindow* window) const {
	glColor4d(m_red, m_green, m_blue, m_alpha);
	glBindTexture(GL_TEXTURE_2D, get_texture_id());
	glPushMatrix();
	transform_gl();
	glBegin(GL_QUADS);
	glTexCoord2d(0.0, 0.0);
	glVertex2i(0,0);
	glTexCoord2d(1.0, 0.0);
	glVertex2i(m_width, 0);
	glTexCoord2d(1.0, 1.0);
	glVertex2i(m_width, m_height);
	glTexCoord2d(0.0, 1.0);
	glVertex2i(0, m_height);
	glEnd();
	glPopMatrix();
}
