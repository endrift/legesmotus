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

Image::Image(const string& path, ResourceCache* cache, bool autogen) {
	stringstream s;
	s << cache->get_root() << "/" << m_image_dir << "/" << path;
	SDL_Surface *image = IMG_Load(s.str().c_str());
	if (image == NULL) {
		throw new Exception("Image could not be loaded");
	}

	m_cache = cache;
	m_width = image->w;
	m_height = image->h;

	m_pitch = image->w*4;

	m_name = path;
	m_handle = 0;

	if (!autogen || image->format->BitsPerPixel != 32) {
		m_pixels = new unsigned char[m_height*m_pitch];
	} else {
		m_pixels = NULL;
	}

	switch (image->format->BitsPerPixel) {
	case 32:
		if (!autogen) {
			memcpy(m_pixels, image->pixels, m_height*m_pitch);
		}
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
		if (image->format->BitsPerPixel == 32) {
			m_pixels = (unsigned char*) image->pixels;
		}

		gen_handle(false);
		
		if (image->format->BitsPerPixel == 32) {
			m_pixels = NULL;
		} else {
			delete_pixels();
		}
	}

	SDL_FreeSurface(image);
}

Image::Image(int width, int height, const string& name, ResourceCache* cache) {
	m_cache = cache;
	m_width = width;
	m_height = height;

	m_pitch = m_width*4;

	m_name = name;

	m_pixels = new unsigned char[m_width*m_pitch];
	m_handle = 0;
}

Image::Image(const Image& other) {
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

	m_cache->increment<Image>(m_name);
}

Image::~Image() {
	int remaining = m_cache->decrement<Image>(m_name);
	if (!remaining) {
		delete[] m_pixels;
		if (m_handle) {
			m_cache->get_context()->del_image(m_handle);
		}
	}
}

void Image::upconvert_8(SDL_Surface* image) {
	unsigned char* pixels = (unsigned char*) image->pixels;
	int tr = 0xFFFFFFFF & image->format->Rmask;
	int tg = 0xFFFFFFFF & image->format->Gmask;
	int tb = 0xFFFFFFFF & image->format->Bmask;
	tr &= image->format->colorkey;
	tg &= image->format->colorkey;
	tb &= image->format->colorkey;
	for (int row = 0; row < m_height; ++row) {
		for (int col = 0; col < m_width; ++col) {
			m_pixels[m_pitch*row + 4*col + 0] = image->format->palette->colors[pixels[image->pitch*row + col]].r;
			m_pixels[m_pitch*row + 4*col + 1] = image->format->palette->colors[pixels[image->pitch*row + col]].g;
			m_pixels[m_pitch*row + 4*col + 2] = image->format->palette->colors[pixels[image->pitch*row + col]].b;
			m_pixels[m_pitch*row + 4*col + 3] = 0xFF;

			if (m_pixels[m_pitch*row + 4*col + 0] == tr && m_pixels[m_pitch*row + 4*col + 1] == tg && m_pixels[m_pitch*row + 4*col + 2] == tb) {
				m_pixels[m_pitch*row + 4*col + 3] = 0;
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
			m_pixels[m_pitch*row + 4*col + 0] = pixels[image->pitch*row + col + r];
			m_pixels[m_pitch*row + 4*col + 1] = pixels[image->pitch*row + col + g];
			m_pixels[m_pitch*row + 4*col + 2] = pixels[image->pitch*row + col + b];
			m_pixels[m_pitch*row + 4*col + 3] = 0xFF;
		}
	}
}

DrawContext::Image Image::gen_handle(bool autofree) {
	if (m_pixels != NULL) {
		m_handle_width = m_width;
		m_handle_height = m_height;
		m_handle = m_cache->get_context()->gen_image(&m_handle_width, &m_handle_height, DrawContext::RGBA, m_pixels);
	}

	if (autofree) {
		delete_pixels();
	}

	return m_handle;
}

DrawContext::Image Image::get_handle() const {
	return m_handle;
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
