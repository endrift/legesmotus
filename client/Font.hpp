/*
 * client/Font.hpp
 *
 * Copyright 2009 - Nathan Partlan, Andrew Ayer, Daniel Schneider, and Jeffrey Pfau
 * 
 */

#ifndef LM_CLIENT_FONT_HPP
#define LM_CLIENT_FONT_HPP

#include <map>
#include "SDL_ttf.h"
#include "Sprite.hpp"

class Font {
private:
	std::map<short,Sprite*>	m_glyph_cache;
	TTF_Font*		m_font;
public:
	Font(const char* filename, int size);
	~Font();

	Sprite* master_glyph(short ch);
	bool glyph_metrics(short ch, int *minx, int *maxx, int *miny, int *maxy, int *advance) const;
};

#endif
