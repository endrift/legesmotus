/*
 * gui/Font.cpp
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

#include "Font.hpp"
#include "client/ConvolveKernel.hpp"
#include "common/Exception.hpp"
#include <cmath>

// XXX remove SDL dependency here
#include "SDL.h"

using namespace LM;
using namespace std;

FT_Library Font::m_library;
bool Font::m_init = false;

Font::Font(const std::string& filename, float size, DrawContext* ctx, bool italic, ConvolveKernel* kernel) {
	// TODO error checking
	FT_Error err;
	if (!m_init) {
		err = FT_Init_FreeType(&m_library);
		if (err) {
			throw new Exception("Could not initialize FreeType");
		}
		m_init = true;
	}
	err = FT_New_Face(m_library, filename.c_str(), 0, &m_face);
	if (err) {
		throw new Exception("Could not initialize font");
	}

	err = FT_Set_Char_Size(m_face, int(size*(1<<6)), 0, 0, 0);
	if (err) {
		throw new Exception("Could not initialize font size");
	}

	m_ctx = ctx;
	m_italic = italic;
	m_kernel = kernel;
}

Font::~Font() {
	for (map<int, Glyph*>::iterator iter = m_glyphs.begin(); iter != m_glyphs.end(); ++iter) {
		delete iter->second;
	}
	FT_Done_Face(m_face);
}

Font::Glyph::Glyph() {
	bitmap_width = 0;
	bitmap_height = 0;
	advance = 0;
	baseline = 0;
	bearing = 0;
	image = 0;
}

Font::Glyph::Glyph(const FT_GlyphSlot& glyph, DrawContext* ctx, bool italic, ConvolveKernel* kernel) {
	FT_Glyph ft_glyph;
	FT_BitmapGlyph ft_bmp;
	m_ctx = ctx;
	bearing = ldexp(glyph->metrics.horiBearingX, -6);
	baseline = ldexp(glyph->metrics.height - glyph->metrics.horiBearingY, -6);
	width = ldexp(glyph->metrics.width, -6);
	height = ldexp(glyph->metrics.height, -6);
	// TODO error checking
	FT_Get_Glyph(glyph, &ft_glyph);
	if (italic) {
		FT_Matrix skew;
		skew.xx = 0x10000;
		skew.xy = 0x04000;
		skew.yx = 0x00000;
		skew.yy = 0x10000;
		FT_Glyph_Transform(ft_glyph, &skew, NULL);
	}
	advance = ldexp(ft_glyph->advance.x, -16);
	FT_Glyph_To_Bitmap(&ft_glyph, FT_RENDER_MODE_NORMAL, NULL, 1);
	ft_bmp = reinterpret_cast<FT_BitmapGlyph>(ft_glyph);
	bitmap_width = ft_bmp->bitmap.pitch;
	bitmap_height = ft_bmp->bitmap.rows;
	if (bitmap_width > 0 && bitmap_height > 0) {
		SDL_Surface* sdl_bmp = NULL;
		unsigned char* bmp = ft_bmp->bitmap.buffer;
		if (kernel != NULL) {
			// FIXME figure out why we pass 2 here and not 1
			SDL_Surface* surf = SDL_CreateRGBSurfaceFrom(bmp, ft_bmp->bitmap.width, bitmap_height, 2, bitmap_width, 0, 0, 0, 0xFF);
			sdl_bmp = kernel->convolve(surf);
			SDL_FreeSurface(surf);
			bitmap_width = sdl_bmp->pitch;
			bitmap_height = sdl_bmp->h;
			bmp = (unsigned char*) sdl_bmp->pixels;
		}
		image = m_ctx->gen_image(&bitmap_width, &bitmap_height, DrawContext::ALPHA, bmp);
		if (sdl_bmp != NULL) {
			SDL_FreeSurface(sdl_bmp);
		}
	} else {
		image = 0;
	}
	FT_Done_Glyph(ft_glyph);
}

Font::Glyph::~Glyph() {
	m_ctx->del_image(image);
}

void Font::Glyph::draw() const {
	if (bitmap_width > 0 && bitmap_height > 0) {
		m_ctx->draw_image(bitmap_width, bitmap_height, image);
	}
}

Font::Glyph* Font::make_glyph(const FT_GlyphSlot& glyph) {
	return new Glyph(m_face->glyph, m_ctx, m_italic, m_kernel);
}

const Font::Glyph* Font::get_glyph(int character) {
	if (m_glyphs.find(character) == m_glyphs.end()) {
		FT_Error err = FT_Load_Char(m_face, character, FT_LOAD_DEFAULT);
		if (err) {
			return NULL;
		}

		Glyph* g = make_glyph(m_face->glyph);
		m_glyphs[character] = g;
	}
	return m_glyphs[character];
}

float Font::get_height() const {
	return ldexp(m_face->size->metrics.height, -6);
}

float Font::kern(int lchar, int rchar) const {
	if (lchar < 0 || rchar < 0) {
		return 0;
	}

	FT_Vector kern;
	kern.x = 0;
	FT_UInt lglyph = FT_Get_Char_Index(m_face, lchar);
	FT_UInt rglyph = FT_Get_Char_Index(m_face, rchar);
	FT_Get_Kerning(m_face, lglyph, rglyph, FT_KERNING_DEFAULT, &kern);
	return ldexp(kern.x, -6);
}

const ConvolveKernel* Font::get_kernel() const {
	return m_kernel;
}
