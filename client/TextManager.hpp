/*
 * client/TextManager.hpp
 *
 * Copyright 2009 - Nathan Partlan, Andrew Ayer, Daniel Schneider, and Jeffrey Pfau
 * 
 */

#ifndef LM_CLIENT_TEXTMANAGER_HPP
#define LM_CLIENT_TEXTMANAGER_HPP

#include "Font.hpp"
#include <vector>

class GameWindow;
class TextManager {
private:
	Font			*m_font;
	std::vector<Sprite*>	m_texts;
	
	double			m_active_red;
	double			m_active_green;
	double			m_active_blue;
	double			m_active_alpha;
public:
	enum Align {
		LEFT = 0,
		CENTER = 1,
		RIGHT = 2
	};
	enum Layer {
		LAYER_HUD,
		LAYER_MAIN
	};
	TextManager(Font* font);
	~TextManager();

	Sprite*	place_string(const std::string& text, double x, double y, Align = LEFT, Layer layer = LAYER_HUD, GameWindow *window = NULL);
	void	reposition_string(Sprite* text, double x, double y, Align = LEFT);
	void	remove_string(Sprite* text, GameWindow* window = NULL);
	void	remove_all_strings(GameWindow* window = NULL);
	void	set_active_color(double r, double g, double b);
	void	set_active_alpha(double a);
	void	set_active_font(Font* font);
};

#endif
