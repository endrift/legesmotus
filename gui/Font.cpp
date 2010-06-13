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
#include "common/Exception.hpp"
#include <cmath>

using namespace LM;
using namespace std;

FT_Library Font::m_library;
bool Font::m_init = false;

Font::Font(const std::string& filename, float size, DrawContext* ctx) {
	// TODO error checking
	FT_Error err;
	if (!m_init) {
		err = FT_Init_FreeType(&m_library);
		if (err) {
			throw new Exception("Could not initialize FreeType");
		}
		m_init = true;
	}

	m_ctx = ctx;
	err = FT_New_Face(m_library, filename.c_str(), 0, &m_face);
	if (err) {
		throw new Exception("Could not initialize font");
	}

	err = FT_Set_Char_Size(m_face, int(size*(1<<6)), 0, 0, 0);
	if (err) {
		throw new Exception("Could not initialize font size");
	}
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

Font::Glyph::Glyph(const FT_GlyphSlot& glyph, DrawContext* ctx) {
	m_ctx = ctx;
	advance = ldexp(glyph->metrics.horiAdvance, -6);
	bearing = ldexp(glyph->metrics.horiBearingX, -6);
	baseline = ldexp(glyph->metrics.height - glyph->metrics.horiBearingY, -6);
	width = ldexp(glyph->metrics.width, -6);
	height = ldexp(glyph->metrics.height, -6);
	bitmap_width = glyph->bitmap.pitch;
	bitmap_height = glyph->bitmap.rows;
	if (bitmap_width > 0 && bitmap_height > 0) {
		image = m_ctx->gen_image(&bitmap_width, &bitmap_height, DrawContext::ALPHA, glyph->bitmap.buffer);
	} else {
		image = 0;
	}
}

Font::Glyph::~Glyph() {
	m_ctx->del_image(image);
}

const Font::Glyph* Font::get_glyph(int character) {
	if (m_glyphs.find(character) == m_glyphs.end()) {
		FT_Error err = FT_Load_Char(m_face, character, FT_LOAD_RENDER);
		if (err) {
			return NULL;
		}

		Glyph* g = new Glyph(m_face->glyph, m_ctx);
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
