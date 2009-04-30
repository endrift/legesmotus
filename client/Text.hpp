/*
 * client/Text.hpp
 *
 * Copyright 2009 - Nathan Partlan, Andrew Ayer, Daniel Schneider, and Jeffrey Pfau
 * 
 */

#ifndef LM_CLIENT_TEXT_HPP
#define LM_CLIENT_TEXT_HPP

#include <string>
#include "Font.hpp"
#include "Graphic.hpp"
#include "Sprite.hpp"

class Text : public Graphic {
private:
	Sprite* m_fg;
	Sprite* m_shadow;
	bool	m_shadow_enabled;

	void	touch_shadow();
public:
	Text(const std::string& text, Font* font);
	virtual ~Text();

	void		set_color(double r, double g, double b);
	virtual void	set_alpha(double a);
	void		set_shadow_color(double r, double g, double b);
	void		set_shadow_alpha(double a);
	void		set_shadow_offset(double x, double y);
	void		set_shadow(bool enable);

	virtual void	set_red_intensity(double r);
	virtual void	set_green_intensity(double g);
	virtual void	set_blue_intensity(double b);

	virtual void	draw(const GameWindow* window) const;
};

#endif
