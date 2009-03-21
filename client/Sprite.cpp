/*
 * client/Sprite.cpp
 *
 * Copyright 2009 - Nathan Partlan, Andrew Ayer, Daniel Schneider, and Jeffrey Pfau
 * 
 */

#include "Sprite.hpp"
#include "SDL_image.h"
#include <stdint.h>

static uint32_t toPow2(uint32_t num) {
	--num;
	for(int i = 1; i < 32; i <<= 1) {
		num |= num >> i;
	}
	++num;
	return num;	
}

Sprite::Sprite(SDL_Surface* image) {
	init(image);
}

Sprite::Sprite(const char* filename) {
	SDL_Surface* loaded = IMG_Load(filename);
	init(loaded);
}

Sprite::Sprite(const Sprite& other) {
	m_x = other.m_x;
	m_y = other.m_y;
	m_scale_x = other.m_scale_x;
	m_scale_y = other.m_scale_y;
	m_image_width = other.m_image_width;
	m_image_height = other.m_image_height;
	m_width = other.m_width;
	m_height = other.m_height;
	m_image = other.m_image;
	m_rotation = other.m_rotation;
	m_center_x = other.m_center_x;
	m_center_y = other.m_center_y;
	m_tex_count = new int;
	*m_tex_count = 1;
}

Sprite::~Sprite() {
	if(*m_tex_count <= 1) {
		glDeleteTextures(1,&m_tex_id);
		SDL_FreeSurface(m_image);
		delete m_tex_count;
	}
}

void Sprite::init(SDL_Surface* image) {
	m_x = 0;
	m_y = 0;
	m_scale_x = 1;
	m_scale_y = 1;
	m_image_width = image->w;
	m_image_height = image->h;
	m_width = toPow2(m_image_width);
	m_height = toPow2(m_image_height);
	m_image = SDL_CreateRGBSurface(SDL_HWSURFACE, m_width, m_height, 32, 0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF);
	SDL_BlitSurface(image, NULL, m_image, NULL);
	m_rotation = 0;
	m_center_x = get_image_width()/2.0;
	m_center_y = get_image_height()/2.0;
	m_tex_count = new int;
	*m_tex_count = 1;
	glGenTextures(1, &m_tex_id);
	glBindTexture(GL_TEXTURE_2D, m_tex_id);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, 4, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, m_image->pixels);
}

GLuint Sprite::get_texture_id() const {
	return m_tex_id;
}

int Sprite::get_width() const {
	return m_width;
}

int Sprite::get_height() const {
	return m_height;
}

int Sprite::get_image_width() const {
	return m_image_width;
}

int Sprite::get_image_height() const {
	return m_image_height;
}

double Sprite::get_x() const {
	return m_x;
}

double Sprite::get_y() const {
	return m_y;
}


double Sprite::get_scale_x() const {
	return m_scale_x;
}

double Sprite::get_scale_y() const {
	return m_scale_y;
}

double Sprite::get_rotation() const {
	return m_rotation;
}

double Sprite::get_center_x() const {
	return m_center_x;
}

double Sprite::get_center_y() const {
	return m_center_y;
}

void Sprite::set_x(double x) {
	m_x = x;
}

void Sprite::set_y(double y) {
	m_y = y;
}

void Sprite::set_scale_x(double scale_x) {
	m_scale_x = scale_x;
}

void Sprite::set_scale_y(double scale_y) {
	m_scale_y = scale_y;
}

void Sprite::set_rotation(double rotation) {
	m_rotation = rotation;
}

void Sprite::set_center_x(double center_x) {
	m_center_x = center_x;
}

void Sprite::set_center_y(double center_y) {
	m_center_y = center_y;
}

void Sprite::set_alpha(double alpha) {
	m_alpha = alpha;
}

void Sprite::draw() const {
	glColor4d(1.0, 1.0, 1.0, m_alpha);
	glBindTexture(GL_TEXTURE_2D,m_tex_id);
	glPushMatrix();
	glTranslated(m_x, m_y, 0.0);
	glRotated(m_rotation, 0.0, 0.0, 1.0);
	glTranslated(-m_center_x, -m_center_y, 0.0);
	glScaled(m_scale_x, m_scale_y, 1.0);
	glBegin(GL_QUADS);
	glTexCoord2d(0.0,0.0);
	glVertex2i(0,0);
	glTexCoord2d(1.0,0.0);
	glVertex2i(m_width,0);
	glTexCoord2d(1.0,1.0);
	glVertex2i(m_width,m_height);
	glTexCoord2d(0.0,1.0);
	glVertex2i(0,m_height);
	glEnd();
	glPopMatrix();
}
