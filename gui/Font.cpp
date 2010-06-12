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
#include <cmath>

using namespace LM;
using namespace std;

FT_Library Font::m_library;
bool Font::m_init = false;

Font::Font(const std::string& filename, float size, DrawContext* ctx) {
	// TODO error checking
	if (!m_init) {
		FT_Init_FreeType(&m_library);
		m_init = true;
	}

	m_ctx = ctx;
	FT_New_Face(m_library, filename.c_str(), 0, &m_face);
	FT_Set_Char_Size(m_face, int(size*(1<<6)), 0, 0, 0);
}

Font::~Font() {
	for (vector<DrawContext::Image>::iterator iter = m_tex.begin(); iter != m_tex.end(); ++iter) {
		m_ctx->del_image(*iter);
	}
	FT_Done_Face(m_face);
	// TODO delete glyphs
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

const Font::Glyph* Font::get_glyph(int character) {
	if (m_glyphs.find(character) == m_glyphs.end()) {
		FT_Load_Char(m_face, character, FT_LOAD_RENDER);
		Glyph* g = new Glyph(m_face->glyph, m_ctx);
		m_tex.push_back(g->image);
		m_glyphs[character] = g;
	}
	return m_glyphs[character];
}

float Font::get_height() const {
	return ldexp(m_face->size->metrics.height, -6);
}
