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
#include "ConvolveKernel.hpp"
#include <cmath>

using namespace LM;
using namespace std;

Label::Label(Font* font) {
	set_font(font);
	m_align = ALIGN_LEFT;
	m_skew_align = VALIGN_MIDDLE;
	m_tracking = 0;
	m_skew = 0;
	m_shadow = NULL;
}

Label::Label(const wstring& str, Font* font) : m_text(str) {
	set_font(font);
	m_align = ALIGN_LEFT;
	m_skew_align = VALIGN_MIDDLE;
	m_tracking = 0;
	m_skew = 0;
	m_shadow = NULL;
	recalculate_width();
}

Label::Label(const string& str, Font* font) : m_text(str.length(), L' ') {
	set_font(font);
	copy(str.begin(), str.end(), m_text.begin());
	m_align = ALIGN_LEFT;
	m_skew_align = VALIGN_MIDDLE;
	m_tracking = 0;
	m_skew = 0;
	m_shadow = NULL;
	recalculate_width();
}

void Label::recalculate_width() {
	float total_advance = 0;
	wchar_t prev_char = -1;

	if (m_font != NULL) {
		for (wstring::const_iterator iter = m_text.begin(); iter != m_text.end(); ++iter) {
			const Font::Glyph* glyph = m_font->get_glyph(*iter);
			float kern = m_font->kern(prev_char, *iter);
			total_advance += glyph->advance + m_tracking + kern;
			prev_char = *iter;
		}
	}

	set_width(total_advance);
}

void Label::set_string(const wstring& str) {
	m_text = str;
	recalculate_width();

	if (m_shadow != NULL) {
		m_shadow->set_string(m_text);
	}
}

void Label::set_string(const string& str) {
	m_text.clear();
	m_text.reserve(str.size());
	copy(str.begin(), str.end(), m_text.begin());
	recalculate_width();

	if (m_shadow != NULL) {
		m_shadow->set_string(m_text);
	}
}

void Label::set_color(Color color) {
	m_color = color;
}

void Label::set_align(Align align) {
	m_align = align;

	if (m_shadow != NULL) {
		m_shadow->set_align(align);
	}
}

Label::Align Label::get_align() const {
	return m_align;
}

void Label::set_tracking(float tracking) {
	m_tracking = tracking;
	recalculate_width();

	if (m_shadow != NULL) {
		m_shadow->set_tracking(tracking);
	}
}

void Label::set_skew(float skew) {
	m_skew = skew;
	//recalculate_width();
	//set_width(get_width()+(fabs(get_font()->get_height()*skew)));

	if (m_shadow != NULL) {
		m_shadow->set_skew(skew);
	}
}

void Label::set_skew_align(VAlign align) {
	m_skew_align = align;

	if (m_shadow != NULL) {
		m_shadow->set_skew_align(align);
	}
}

Label::VAlign Label::get_skew_align() const {
	return m_skew_align;
}

void Label::set_shadow(Label* shadow) {
	if (shadow != this) {
		m_shadow = shadow;
		if (shadow != NULL) {
			m_shadow->set_string(m_text);
			m_shadow->set_tracking(m_tracking);
			m_shadow->set_align(m_align);
		}
	}
}

Label* Label::get_shadow() {
	return m_shadow;
}

void Label::set_font(Font* font) {
	m_font = font;
	if (font != NULL) {
		set_height(font->get_height());
	}
	recalculate_width();
}

const Font* Label::get_font() const {
	return m_font;
}

void Label::draw(DrawContext* ctx) const {
	if (m_shadow != NULL) {
		m_shadow->draw(ctx);
	}

	if (m_font == NULL) {
		return;
	}

	float total_advance = 0;
	float align = 0;
	float valign = 0;
	wchar_t prev_char = -1;
	const ConvolveKernel* kernel = get_font()->get_kernel();

	if (get_skew_align() == VALIGN_TOP) {
		valign = get_font()->get_height();
	} else if (get_skew_align() == VALIGN_MIDDLE) {
		valign = get_font()->get_height() * 0.5f;
	}

	if (get_align() == ALIGN_CENTER) {
		align = (get_width() + m_skew*valign) * 0.5f;
	} else if (get_align() == ALIGN_RIGHT) {
		align = get_width() + m_skew*valign;
	}

	ctx->skew_x(m_skew);

	if (kernel != NULL) {
		ctx->translate(-kernel->get_width() / 2.0, -kernel->get_height() / 2.0);
	}

	ctx->set_draw_color(m_color);
	ctx->translate(get_x() - align, get_y());

	for (wstring::const_iterator iter = m_text.begin(); iter != m_text.end(); ++iter) {
		const Font::Glyph* glyph = m_font->get_glyph(*iter);
		float kern = m_font->kern(prev_char, *iter);
		float advance = glyph->advance + m_tracking + kern;
		int height = - glyph->height + glyph->baseline;
		ctx->translate(glyph->bearing, height);
		glyph->draw();
		ctx->translate(advance - glyph->bearing, -height);
		total_advance += advance;
		prev_char = *iter;
	}

	ctx->translate(align - (total_advance + get_x()), -get_y());

	if (kernel != NULL) {
		ctx->translate(kernel->get_width() / 2.0, kernel->get_height() / 2.0);
	}

	ctx->skew_x(-m_skew);
}
