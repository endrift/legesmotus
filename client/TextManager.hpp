/*
 * client/TextManager.hpp
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

#ifndef LM_CLIENT_TEXTMANAGER_HPP
#define LM_CLIENT_TEXTMANAGER_HPP

#include "Font.hpp"
#include "Text.hpp"
#include <vector>

class GameWindow;
class TextManager {
private:
	Font*			m_font;
	std::vector<Text*>	m_texts;
	GameWindow*		m_window;
	
	double			m_active_red;
	double			m_active_green;
	double			m_active_blue;
	double			m_active_alpha;

	bool			m_shadow;
	double			m_shadow_x;
	double			m_shadow_y;
	double			m_shadow_red;
	double			m_shadow_green;
	double			m_shadow_blue;
	double			m_shadow_alpha;
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
	TextManager(Font* font, GameWindow* window = NULL);
	~TextManager();

	Text*	place_string(const std::string& text, double x, double y, Align = LEFT, Layer layer = LAYER_HUD);
	void	reposition_string(Graphic* text, double x, double y, Align = LEFT);
	void	remove_string(Graphic* text);
	void	remove_all_strings();
	void	set_window(GameWindow* window);
	void	set_active_color(double r, double g, double b);
	void	set_active_color(Color c);
	void	set_active_alpha(double a);
	void	set_active_font(Font* font);
	void	set_shadow_color(double r, double g, double b);
	void	set_shadow_alpha(double a);
	void	set_shadow_offset(double x, double y);
	void	set_shadow(bool enable);
};

#endif
