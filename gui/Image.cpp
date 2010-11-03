/*
 * gui/Image.cpp
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

#include "Image.hpp"
#include "ResourceCache.hpp"
#include "common/Exception.hpp"

// XXX remove SDL_Image dependency
#include "SDL_image.h"

// Why oh why do you exist, STL?
#include <sstream>

using namespace LM;
using namespace std;

const char* Image::m_image_dir = "sprites";

Image::Image() {
	m_cache = NULL;
	m_owns_handle = false;
	m_handle = 0;
	m_width = 0;
	m_height = 0;
	m_pitch = 0;
	m_handle_width = 0;
	m_handle_height = 0;
	m_pixels = NULL;
}

Image::Image(const string& path, ResourceCache* cache, bool autogen) {
	Image *self = cache->get<Image>(path);
	m_owns_handle = true;
	if (self != NULL) {
		m_cache = NULL;
		*this = *self;
	} else {
		m_cache = cache;
		m_name = path;
		reload(autogen);
		m_cache->add(path, *this);
	}
}

Image::Image(int width, int height, const string& name, ResourceCache* cache, DrawContext::Image handle) {
	m_cache = cache;
	m_width = width;
	m_height = height;
	m_handle_width = width;
	m_handle_height = height;

	m_pitch = m_width*4;

	m_name = name;

	if (handle) {
		m_pixels = NULL;
		m_handle = handle;
		m_owns_handle = false;
	} else {
		m_pixels = new unsigned char[m_width*m_pitch];
		m_handle = 0;
		m_owns_handle = true;
	}
}

Image::Image(const Image& other) {
	m_cache = NULL;
	*this = other;
}

Image::~Image() {
	if (m_cache != NULL) {
		int remaining = m_cache->decrement<Image>(m_name);
		if (!remaining) {
			delete[] m_pixels;
			if (m_handle && m_owns_handle) {
				m_cache->get_context()->del_image(m_handle);
			}
		}
	}
}

void Image::upconvert_8(SDL_Surface* image) {
	unsigned char* pixels = (unsigned char*) image->pixels;
	for (int row = 0; row < m_height; ++row) {
		for (int col = 0; col < m_width; ++col) {
			if (image->flags & SDL_SRCCOLORKEY && pixels[image->pitch*row + col] == image->format->colorkey) {
				m_pixels[m_pitch*row + 4*col + 3] = 0;
			} else {
				m_pixels[m_pitch*row + 4*col + 0] = image->format->palette->colors[pixels[image->pitch*row + col]].r;
				m_pixels[m_pitch*row + 4*col + 1] = image->format->palette->colors[pixels[image->pitch*row + col]].g;
				m_pixels[m_pitch*row + 4*col + 2] = image->format->palette->colors[pixels[image->pitch*row + col]].b;
				m_pixels[m_pitch*row + 4*col + 3] = 0xFF;
			}
		}
	}
}

void Image::upconvert_24(SDL_Surface* image) {
	unsigned char* pixels = (unsigned char*) image->pixels;
	int r = image->format->Rshift >> 3;
	int g = image->format->Gshift >> 3;
	int b = image->format->Bshift >> 3;

	for (int row = 0; row < m_height; ++row) {
		for (int col = 0; col < m_width; ++col) {
			m_pixels[m_pitch*row + 4*col + 0] = pixels[image->pitch*row + 3*col + r];
			m_pixels[m_pitch*row + 4*col + 1] = pixels[image->pitch*row + 3*col + g];
			m_pixels[m_pitch*row + 4*col + 2] = pixels[image->pitch*row + 3*col + b];
			m_pixels[m_pitch*row + 4*col + 3] = 0xFF;
		}
	}
}

void Image::rearrange_32(SDL_Surface* image) {
	unsigned char* pixels = (unsigned char*) image->pixels;

	int aloss = (1 << image->format->Aloss) - 1;

	for (int row = 0; row < m_height; ++row) {
		for (int col = 0; col < m_width; ++col) {
			int* pixel = (int*)&pixels[image->pitch*row + 4*col];
			m_pixels[m_pitch*row + 4*col + 0] = (*pixel & image->format->Rmask)>>image->format->Rshift;
			m_pixels[m_pitch*row + 4*col + 1] = (*pixel & image->format->Gmask)>>image->format->Gshift;
			m_pixels[m_pitch*row + 4*col + 2] = (*pixel & image->format->Bmask)>>image->format->Bshift;
			m_pixels[m_pitch*row + 4*col + 3] = ((*pixel & image->format->Amask)>>image->format->Ashift) + aloss;
		}
	}
}

DrawContext::Image Image::gen_handle(bool autofree) {
	if (m_pixels != NULL) {
		m_handle_width = m_width;
		m_handle_height = m_height;
		m_handle = m_cache->get_context()->gen_image(&m_handle_width, &m_handle_height, DrawContext::RGBA, m_pixels);
		m_owns_handle = true;
	}

	if (autofree) {
		delete_pixels();
	}

	return m_handle;
}

DrawContext::Image Image::get_handle() const {
	return m_handle;
}

void Image::reload(bool autogen) {
	stringstream s;
	s << m_cache->get_root() << "/" << m_image_dir << "/" << m_name;
	SDL_Surface *image = IMG_Load(s.str().c_str());
	if (image == NULL) {
		throw new Exception("Image could not be loaded");
	}

	m_width = image->w;
	m_height = image->h;

	m_pitch = image->w*4;

	m_handle = 0;

	m_pixels = new unsigned char[m_height*m_pitch];

	switch (image->format->BitsPerPixel) {
	case 32:
		rearrange_32(image);
		break;

	case 24:
		upconvert_24(image);
		break;

	case 8:
		upconvert_8(image);
		break;

	default:
		throw new Exception("Can't handle unknown image depth");
	}

	if (autogen) {
		gen_handle(false);
		delete_pixels();
	}

	SDL_FreeSurface(image);
}

void Image::delete_pixels() {
	Image* master = m_cache->get<Image>(m_name);
	if (master == this || master == NULL) {
		delete[] m_pixels;
	} else {
		master->delete_pixels();
	}

	m_pixels = NULL;
}

void Image::add_mipmap(const std::string& name, int level) {
	Image image(name, m_cache, false);
	add_mipmap(image, level);
}

void Image::add_mipmap(const Image& image, int level) {
	gen_handle();
	int w = image.get_width();
	int h = image.get_height();
	m_cache->get_context()->add_mipmap(get_handle(), level, &w, &h, DrawContext::RGBA, image.get_pixels());
}

int Image::get_width() const {
	return m_width;
}

int Image::get_height() const {
	return m_height;
}

int Image::get_pitch() const {
	return m_pitch;
}

int Image::get_handle_width() const {
	return m_handle_width;
}

int Image::get_handle_height() const {
	return m_handle_height;
}

const unsigned char* Image::get_pixels() const {
	Image* master = m_cache->get<Image>(m_name);
	if (master == this || master == NULL) {
		return m_pixels;
	} else {
		return master->get_pixels();
	}
}

unsigned char* Image::get_pixels() {
	Image* master = m_cache->get<Image>(m_name);
	if (master != this && master != NULL) {
		m_pixels = master->get_pixels();
	}
	return m_pixels;
}

Image& Image::operator=(const Image& other) {
	if (&other != this) {
		if (m_cache != NULL) {
			m_cache->decrement<Image>(m_name);
		}
	
		m_cache = other.m_cache;
		m_width = other.m_width;
		m_height = other.m_height;
		m_pitch = other.m_pitch;
		m_pixels = other.m_pixels;
		m_handle = other.m_handle;
		m_name = other.m_name;
	
		m_handle = other.m_handle;
		m_handle_width = other.m_handle_width;
		m_handle_height = other.m_handle_height;
		m_owns_handle = false;
	
		m_cache->increment<Image>(m_name);
	}

	return *this;
}
