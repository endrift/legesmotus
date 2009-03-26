/*
 * client/Sprite.hpp
 *
 * Copyright 2009 - Nathan Partlan, Andrew Ayer, Daniel Schneider, and Jeffrey Pfau
 * 
 */

#ifndef LM_CLIENT_SPRITE_HPP
#define LM_CLIENT_SPRITE_HPP

#include "Graphic.hpp"

class Sprite : public Graphic {
private:
	int		m_width;
	int		m_height;

	double		m_x;
	double		m_y;

	double		m_scale_x;
	double		m_scale_y;
	double		m_rotation;

	double		m_center_x;
	double		m_center_y;

	double		m_alpha;
	double		m_red;
	double		m_green;
	double		m_blue;

	void		init();

public:
	Sprite(SDL_Surface* image);
	Sprite(const char* filename);
	Sprite(const Sprite& other);

	int	get_width() const;
	int	get_height() const;
	double	get_x() const;
	double	get_y() const;

	double	get_scale_x() const;
	double	get_scale_y() const;
	double	get_rotation() const;

	double	get_center_x() const;
	double	get_center_y() const;

	double	get_alpha() const;
	double	get_red_intensity() const;
	double	get_green_intensity() const;
	double	get_blue_intensity() const;

	void	set_x(double x);
	void	set_y(double y);

	void	set_scale_x(double scale_x);
	void	set_scale_y(double scale_y);
	void	set_rotation(double rotation);

	void	set_center_x(double center_x);
	void	set_center_y(double center_y);

	void	set_alpha(double alpha);
	void	set_red_intensity(double r);
	void	set_green_intensity(double g);
	void	set_blue_intensity(double b);

	void	draw(const GameWindow* window) const;
};

#endif
