/*
 * client/Font.cpp
 *
 * This file is part of Leges Motus, a networked, 2D shooter set in zero gravity.
 * 
 * Copyright 2009 Andrew Ayer, Nathan Partlan, Jeffrey Pfau
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

using namespace LM;
using namespace std;

Font::Font(const char* filename, int size) {
	if (!TTF_WasInit() && TTF_Init() == -1) {
		throw Exception("Could not init SDL_TTF!");
	}
	m_font = TTF_OpenFont(filename, size);
	if (!m_font) {
		throw Exception("Could not load font!");
	}
}

Font::~Font() {
	if (m_font != NULL) {
		TTF_CloseFont(m_font);
	}
}

void Font::set_font_style(bool bold, bool italic) {
	if (bold && italic) {
		TTF_SetFontStyle(m_font, TTF_STYLE_BOLD|TTF_STYLE_ITALIC);
	} else if (bold) {
		TTF_SetFontStyle(m_font, TTF_STYLE_BOLD);
	} else if (italic) {
		TTF_SetFontStyle(m_font, TTF_STYLE_ITALIC);
	} else {
		TTF_SetFontStyle(m_font, TTF_STYLE_NORMAL);
	}
}

Sprite* Font::render_string(const string& text, const ConvolveKernel* kernel) {
	SDL_Color white = { 255, 255, 255, 0 };
	SDL_Surface* rendered = TTF_RenderUTF8_Blended(m_font, text.c_str(), white);
	if (rendered == NULL) {
		return NULL;
	}
	if (kernel != NULL) {
		SDL_Surface* convolved = kernel->convolve(rendered);
		SDL_FreeSurface(rendered);
		rendered = convolved;
	}
	Sprite* text_sprite = new Sprite(rendered);
	SDL_FreeSurface(rendered);
	return text_sprite;
}

bool Font::size_string(const string& text, int* w, int* h) {
	return TTF_SizeText(m_font, text.c_str(), w, h) == 0;
}

int Font::line_skip() const {
	return TTF_FontLineSkip(m_font);
}

int Font::ascent() const {
	return TTF_FontAscent(m_font);
}

int Font::descent() const {
	return TTF_FontDescent(m_font);
}
