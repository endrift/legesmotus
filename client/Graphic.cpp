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

Graphic::Graphic() {
	init(NULL);
}

Graphic::Graphic(SDL_Surface* image) {
	init(image);
}

Graphic::Graphic(const char* filename) {
	SDL_Surface* loaded = IMG_Load(filename);
	init(loaded);
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
	m_image = other.m_image;
	m_tex_id = other.m_tex_id;
	m_tex_count = other.m_tex_count;
	if (m_tex_count != NULL) {
		++*m_tex_count;
	}
}

Graphic::~Graphic() {
	if (m_tex_count != NULL && *m_tex_count <= 1) {
		glDeleteTextures(1,&m_tex_id);
		SDL_FreeSurface(m_image);
		delete m_tex_count;
	}
}

void Graphic::init(SDL_Surface* image) {
	m_image_width = 0;
	m_image_height = 0;
	m_center_x = 0.0;
	m_center_y = 0.0;
	m_invisible = false;
	m_tex_count = NULL;
	m_image = NULL;
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
	int width = toPow2(image->w);
	int height = toPow2(image->h);
	m_image = SDL_CreateRGBSurface(SDL_HWSURFACE, width, height, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
	SDL_SetAlpha(image, 0, SDL_ALPHA_OPAQUE);
	SDL_BlitSurface(image, NULL, m_image, NULL);
	m_tex_count = new int;
	*m_tex_count = 1;
	glGenTextures(1, &m_tex_id);
	glBindTexture(GL_TEXTURE_2D, m_tex_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, 4, width, height, 0, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, m_image->pixels);
}

GLuint Graphic::get_texture_id() const {
	return m_tex_id;
}

void Graphic::transform_gl() const {
	glBegin(GL_POINTS);
	glEnd();
	glTranslated(round(m_x), round(m_y), 0.0); //TODO find alternative method
	glRotated(m_rotation, 0.0, 0.0, 1.0);
	glScaled(m_scale_x, m_scale_y, 1.0);
	glTranslated(-round(m_center_x), -round(m_center_y), 0.0);
	glVertex3i(0,0,-200);
}

int Graphic::get_image_width() const {
	return m_image_width;
}

int Graphic::get_image_height() const {
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

bool Graphic::is_invisible() const {
	return m_invisible;
}

void Graphic::set_invisible(bool invisible) {
	m_invisible = invisible;
}
