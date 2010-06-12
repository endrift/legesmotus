/*
 * gui/Label.cpp
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

#include "Label.hpp"
#include <cmath>

using namespace LM;
using namespace std;

Label::Label(Font* font) {
	m_font = font;
	set_height(font->get_height());
	m_align = ALIGN_LEFT;
}

Label::Label(const wstring& str, Font* font) : m_text(str) {
	m_font = font;
	set_height(font->get_height());
	m_align = ALIGN_LEFT;
	recalculate_width();
}

Label::Label(const string& str, Font* font) : m_text(str.length(), L' ') {
	m_font = font;
	set_height(font->get_height());
	copy(str.begin(), str.end(), m_text.begin());
	m_align = ALIGN_LEFT;
	recalculate_width();
}

void Label::recalculate_width() {
	int total_advance = 0;
	float residual_advance = 0.0f;

	for (wstring::const_iterator iter = m_text.begin(); iter != m_text.end(); ++iter) {
		const Font::Glyph* glyph = m_font->get_glyph(*iter);
		int advance;
		float advancef;
		residual_advance += modf(glyph->advance, &advancef);
		advance = advancef;
		total_advance += advance;
	}

	set_width(total_advance + residual_advance);
}

void Label::set_align(Align align) {
	m_align = align;
}

Label::Align Label::get_align() const {
	return m_align;
}

void Label::redraw(DrawContext* ctx) const {
	int total_advance = 0;
	float residual_advance = 0.0f;
	int align = 0;

	if (get_align() == ALIGN_CENTER) {
		align = get_width() * 0.5f;
	} else if (get_align() == ALIGN_RIGHT) {
		align = get_width();
	}

	ctx->translate(get_x() - align, get_y());

	for (wstring::const_iterator iter = m_text.begin(); iter != m_text.end(); ++iter) {
		const Font::Glyph* glyph = m_font->get_glyph(*iter);
		int advance;
		float advancef;
		residual_advance += modf(glyph->advance, &advancef);
		advance = advancef;
		int height = - glyph->height + glyph->baseline;
		ctx->translate(glyph->bearing, height);
		if (glyph->bitmap_width > 0 && glyph->bitmap_height > 0) {
			ctx->draw_image(glyph->bitmap_width, glyph->bitmap_height, glyph->image);
		}
		ctx->translate(advance - int(glyph->bearing), -height);
		total_advance += advance;
	}

	ctx->translate(align - (total_advance + get_x()), -get_y());
}
