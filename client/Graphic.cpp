/*
 * client/Graphic.cpp
 *
 * Copyright 2009 - Nathan Partlan, Andrew Ayer, Daniel Schneider, and Jeffrey Pfau
 * 
 */

#include "Graphic.hpp"
#include "SDL_image.h"
#include "common/LMException.hpp"
#include "common/math.hpp"

Graphic::Graphic(SDL_Surface* image) {
	init(image);
}

Graphic::Graphic(const char* filename) {
	SDL_Surface* loaded = IMG_Load(filename);
	init(loaded);
}

Graphic::Graphic(const Graphic& other) {
	m_priority = other.m_priority;
	m_image_width = other.m_image_width;
	m_image_height = other.m_image_height;
	m_image = other.m_image;
	m_tex_id = other.m_tex_id;
	m_tex_count = other.m_tex_count;
	++*m_tex_count;
}

Graphic::~Graphic() {
	if(*m_tex_count <= 1) {
		glDeleteTextures(1,&m_tex_id);
		SDL_FreeSurface(m_image);
		delete m_tex_count;
	}
}

void Graphic::init(SDL_Surface* image) {
	if (image == NULL) {
		throw LMException("Sprite could not be loaded");
	}
	m_priority = 0;
	m_image_width = image->w;
	m_image_height = image->h;
	int width = toPow2(image->w);
	int height = toPow2(image->h);
	m_image = SDL_CreateRGBSurface(SDL_HWSURFACE, width, height, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
	SDL_SetAlpha(image, 0, SDL_ALPHA_OPAQUE);
	SDL_BlitSurface(image, NULL, m_image, NULL);
	m_tex_count = new int;
	*m_tex_count = 1;
	glGenTextures(1, &m_tex_id);
	glBindTexture(GL_TEXTURE_2D, m_tex_id);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, 4, width, height, 0, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, m_image->pixels);
}

GLuint Graphic::get_texture_id() const {
	return m_tex_id;
}

int Graphic::get_image_width() const {
	return m_image_width;
}

int Graphic::get_image_height() const {
	return m_image_height;
}

int Graphic::get_priority() const {
	return m_priority;
}

void Graphic::set_priority(int priority) {
	m_priority = priority;
}
