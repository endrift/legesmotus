/*
 * gui/Image.hpp
 *
 * This file is part of Leges Motus, a networked, 2D shooter set in zero gravity.
 * 
 * Copyright 2009-2011 Andrew Ayer, Nathan Partlan, Jeffrey Pfau
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

#ifndef LM_GUI_IMAGE_HPP
#define LM_GUI_IMAGE_HPP

#include "DrawContext.hpp"

#include <string>

// XXX remove SDL_Image dependency
#include "SDL.h"

namespace LM {
	class ResourceCache;

	class Image {
	private:
		static const char* m_image_dir;

		ResourceCache*	m_cache;
		std::string		m_name;

		int m_width;
		int m_height;
		int m_pitch;

		int m_handle_width;
		int m_handle_height;

		unsigned char*		m_pixels;
		DrawContext::Image	m_handle;
		bool				m_owns_handle;

		void upconvert_alpha(int p, unsigned char* d);
		void upconvert_8(SDL_Surface* image);
		void upconvert_24(SDL_Surface* image);
		void rearrange_32(SDL_Surface* image);

	public:
		Image();
		Image(const std::string& path, ResourceCache* cache, bool autogen = false);
		Image(int width, int height, const std::string& name, ResourceCache* cache, DrawContext::Image handle = 0);
		Image(int width, int height, int pitch, DrawContext::PixelFormat fmt, const std::string& name, ResourceCache* cache, unsigned char* pixels);
		Image(const Image& other);
		~Image();

		DrawContext::Image gen_handle(bool autofree = true, DrawContext* ctx = NULL);
		DrawContext::Image get_handle() const;

		void reload(bool autogen = false);
		void delete_pixels();

		void add_mipmap(const std::string& name, int level);
		void add_mipmap(const Image& image, int level);

		int get_width() const;
		int get_height() const;
		int get_pitch() const;

		int get_handle_width() const;
		int get_handle_height() const;

		const unsigned char* get_pixels() const;
		unsigned char* get_pixels();

		Image& operator=(const Image& other);
	};
}

#endif
