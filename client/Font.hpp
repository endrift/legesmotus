/*
 * client/Font.hpp
 *
 * Copyright 2009 - Nathan Partlan, Andrew Ayer, Daniel Schneider, and Jeffrey Pfau
 * 
 */

#ifndef LM_CLIENT_FONT_HPP
#define LM_CLIENT_FONT_HPP

#include "SDL_ttf.h"
#include "Sprite.hpp"
#include <string>
#include <map>

class Font {
private:
	TTF_Font*	m_font;
public:
	Font(const char* filename, int size);

	Sprite*	render_string(const std::string& text);
	int	line_skip() const;
	int	ascent() const;
	int	descent() const;
};

#endif
