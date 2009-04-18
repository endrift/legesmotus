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
	if(!TTF_WasInit() && TTF_Init() == -1) {
		throw LMException("Could not init SDL_TTF!");
	}
	m_font = TTF_OpenFont(filename, size);
	if(!m_font) {
		throw LMException("Could not load font!");
	}
}

Font::~Font() {
	for(map<short,Sprite*>::iterator iter = m_glyph_cache.begin(); iter != m_glyph_cache.end(); ++iter) {
		delete iter->second;
	}
}

Sprite* Font::master_glyph(short ch) {
	Sprite *glyph = m_glyph_cache[ch];
	if(glyph == NULL) {
		SDL_Color color = { 255, 255, 255 };
		SDL_Surface *image = TTF_RenderGlyph_Blended(m_font,ch,color);
		if(image != NULL) {
			glyph = new Sprite(image);
			m_glyph_cache[ch] = glyph;
		}
	}
	return glyph;
}

bool Font::glyph_metrics(short ch, int *minx, int *maxx, int *miny, int *maxy, int *advance) const {
	return TTF_GlyphMetrics(m_font, ch, minx, maxx, miny, maxy, advance) == 0;
}
