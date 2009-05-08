/*
 * client/TiledGraphic.cpp
 *
 * Copyright 2009 - Nathan Partlan, Andrew Ayer, Daniel Schneider, and Jeffrey Pfau
 * 
 */

#include "TiledGraphic.hpp"
#include "GameWindow.hpp"
#include "common/math.hpp"

TiledGraphic::TiledGraphic(SDL_Surface* image) : Graphic(image) {
	init();
}

TiledGraphic::TiledGraphic(const char* filename) : Graphic(filename) {	
	init();
}

TiledGraphic::TiledGraphic(const TiledGraphic& other) : Graphic(other) {
	m_start_x = other.m_start_x;
	m_start_y = other.m_start_y;
	m_red = other.m_red;
	m_green = other.m_green;
	m_blue = other.m_blue;
	m_alpha = other.m_alpha;
	m_width = other.m_width;
	m_height = other.m_height;
	m_tile_width = other.m_tile_width;
	m_tile_height = other.m_tile_height;
}

TiledGraphic* TiledGraphic::clone() const {
	return new TiledGraphic(*this);
}

void TiledGraphic::init() {
	m_start_x = 0;
	m_start_y = 0;
	m_red = 1.0;
	m_green = 1.0;
	m_blue = 1.0;
	m_alpha = 1.0;
	m_tile_width = toPow2(get_image_width());
	m_tile_height = toPow2(get_image_height());
	m_width = m_tile_width;
	m_height = m_tile_height;
}

double TiledGraphic::get_width() const {
	return m_width;
}

double TiledGraphic::get_height() const {
	return m_height;
}

double TiledGraphic::get_start_x() const {
	return m_start_x;
}

double TiledGraphic::get_start_y() const {
	return m_start_y;
}

double TiledGraphic::get_tile_width() const {
	return m_tile_width;
}

double TiledGraphic::get_tile_height() const {
	return m_tile_height;
}

void TiledGraphic::set_width(double width) {
	m_width = width;
}

void TiledGraphic::set_height(double height) {
	m_height = height;
}

void TiledGraphic::set_start_x(double start_x) {
	m_start_x = start_x;
}

void TiledGraphic::set_start_y(double start_y) {
	m_start_y = start_y;
}

void TiledGraphic::set_tile_width(double tile_width) {
	m_tile_width = tile_width;
}

void TiledGraphic::set_tile_height(double tile_height) {
	m_tile_height = tile_height;
}

double TiledGraphic::get_alpha() const {
	return m_alpha;
}

double TiledGraphic::get_red_intensity() const {
	return m_red;
}

double TiledGraphic::get_green_intensity() const {
	return m_green;
}

double TiledGraphic::get_blue_intensity() const {
	return m_blue;
}

void TiledGraphic::set_alpha(double alpha) {
	m_alpha = alpha;
}

void TiledGraphic::set_red_intensity(double red) {
	m_red = red;
}

void TiledGraphic::set_green_intensity(double green) {
	m_green = green;
}

void TiledGraphic::set_blue_intensity(double blue) {
	m_blue = blue;
}

void TiledGraphic::draw(const GameWindow* window) const {
	(void)(window); // Unused
	glColor4d(m_red, m_green, m_blue, m_alpha);
	glBindTexture(GL_TEXTURE_2D,get_texture_id());
	glPushMatrix();
	transform_gl();
	glTranslated(m_start_x, m_start_y, 0.0);
	glBegin(GL_QUADS);
	glTexCoord2d(0.0,0.0);
	glVertex2i(0,0);
	glTexCoord2d(m_width/m_tile_width,0.0);
	glVertex2i(int(m_width),0);
	glTexCoord2d(m_width/m_tile_width,m_height/m_tile_height);
	glVertex2i(int(m_width),int(m_height));
	glTexCoord2d(0.0,m_height/m_tile_height);
	glVertex2i(0,int(m_height));
	glEnd();
	glPopMatrix();
}
