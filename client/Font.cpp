/*
 * client/Font.cpp
 *
 * Copyright 2009 - Nathan Partlan, Andrew Ayer, Daniel Schneider, and Jeffrey Pfau
 * 
 */

#include "Font.hpp"
#include "common/LMException.hpp"

using namespace std;

Font::Font(const char* filename, int size) {
	if (!TTF_WasInit() && TTF_Init() == -1) {
		throw LMException("Could not init SDL_TTF!");
	}
	m_font = TTF_OpenFont(filename, size);
	if (!m_font) {
		throw LMException("Could not load font!");
	}
}

Font::~Font() {
	if (m_font != NULL) {
		TTF_CloseFont(m_font);
	}
}

Sprite* Font::render_string(const string& text) {
	SDL_Color white = { 255, 255, 255, 0 };
	SDL_Surface* rendered = TTF_RenderUTF8_Blended(m_font,text.c_str(),white);
	if(!rendered) return NULL;
	return new Sprite(rendered);
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
