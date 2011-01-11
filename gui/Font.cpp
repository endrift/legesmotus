/*
 * gui/Font.cpp
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

#include "Font.hpp"
#include "ConvolveKernel.hpp"
#include "common/Exception.hpp"
#include "ResourceCache.hpp"
#include <cmath>
#include <sstream>

using namespace LM;
using namespace std;

FT_Library Font::m_library;
bool Font::m_init = false;

string Font::lookup_id(const std::string& filename, float size, bool italic, const ConvolveKernel* kernel) {
	stringstream font_name;
	font_name << "font:" << filename << "@" << size << (italic?"i":"") << "/" << hex << kernel;
	return font_name.str();
}

Font::Font(const std::string& filename, float size, ResourceCache* cache, bool italic, const ConvolveKernel* kernel) {
	// TODO error checking
	FT_Error err;
	if (!m_init) {
		err = FT_Init_FreeType(&m_library);
		if (err) {
			throw new Exception("Could not initialize FreeType");
		}
		m_init = true;
	}

	m_font_name = lookup_id(filename, size, italic, kernel);
	// If this assert fails, we need operator= like in Image
	ASSERT(cache->get<Font>(m_font_name) == NULL);

	err = FT_New_Face(m_library, (cache->get_root() + "/fonts/" + filename).c_str(), 0, &m_face);
	if (err) {
		throw new Exception("Could not initialize font");
	}

	err = FT_Set_Char_Size(m_face, int(size*(1<<6)), 0, 0, 0);
	if (err) {
		throw new Exception("Could not initialize font size");
	}

	m_cache = cache;
	m_italic = italic;
	m_kernel = kernel;
	m_glyphs = new map<int, Glyph*>;

	m_cache->add(m_font_name, *this);
}

Font::Font(const Font& other) {
	m_font_name = other.m_font_name;
	m_face = other.m_face;
	m_glyphs = other.m_glyphs;
	m_cache = other.m_cache;
	m_kernel = other.m_kernel;
	m_italic = other.m_italic;
}

Font::~Font() {
	int remaining = m_cache->decrement<Font>(m_font_name);
	if (!remaining) {
		delete m_glyphs;
		FT_Done_Face(m_face);
	}
}

Font::Glyph::Glyph() {
	bitmap_width = 0;
	bitmap_height = 0;
	advance = 0;
	baseline = 0;
	bearing = 0;
}

Font::Glyph::Glyph(const FT_GlyphSlot& glyph, DrawContext* ctx, bool italic, const ConvolveKernel* kernel) {
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
		unsigned char* bmp = ft_bmp->bitmap.buffer;
		Image surf(ft_bmp->bitmap.width, bitmap_height, bitmap_width, DrawContext::ALPHA, "", NULL, bmp);
		if (kernel != NULL) {
			image = kernel->convolve(surf);
		} else {
			image = surf;
		}
		image.gen_handle(true, m_ctx);

		bitmap_width = image.get_handle_width();
		bitmap_height = image.get_handle_height();
	}
	FT_Done_Glyph(ft_glyph);
}

Font::Glyph::~Glyph() {
	// Nothing to do
}

void Font::Glyph::draw() const {
	if (bitmap_width > 0 && bitmap_height > 0) {
		m_ctx->draw_image(bitmap_width, bitmap_height, image.get_handle());
	}
}

Font::Glyph* Font::make_glyph(const FT_GlyphSlot& glyph) {
	return new Glyph(m_face->glyph, m_cache->get_context(), m_italic, m_kernel);
}

const string& Font::get_id() const {
	return m_font_name;
}

const Font::Glyph* Font::get_glyph(int character) {
	if (m_glyphs->find(character) == m_glyphs->end()) {
		FT_Error err = FT_Load_Char(m_face, character, FT_LOAD_DEFAULT);
		if (err) {
			return NULL;
		}

		Glyph* g = make_glyph(m_face->glyph);
		(*m_glyphs)[character] = g;
	}
	return (*m_glyphs)[character];
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

void Font::increment() {
	m_cache->increment<Font>(m_font_name);
}

void Font::decrement() {
	m_cache->decrement<Font>(m_font_name);
}
