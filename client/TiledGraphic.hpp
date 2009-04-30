/*
 * client/TiledGraphic.hpp
 *
 * Copyright 2009 - Nathan Partlan, Andrew Ayer, Daniel Schneider, and Jeffrey Pfau
 * 
 */

#ifndef LM_CLIENT_TILEDGRAPHIC_HPP
#define LM_CLIENT_TILEDGRAPHIC_HPP

#include "Graphic.hpp"

class TiledGraphic : public Graphic {
private:
	double	m_start_x;
	double	m_start_y;
	double	m_width;
	double	m_height;
	double	m_tile_width;
	double	m_tile_height;

	double	m_alpha;
	double	m_red;
	double	m_green;
	double	m_blue;

	void	init();

public:
	TiledGraphic(SDL_Surface* image);
	TiledGraphic(const char* filename);
	TiledGraphic(const TiledGraphic& other);
	virtual TiledGraphic* clone() const;

	double	get_width() const;
	double	get_height() const;
	double	get_start_x() const;
	double	get_start_y() const;
	double	get_tile_width() const;
	double	get_tile_height() const;

	void	set_width(double width);
	void	set_height(double height);
	void	set_start_x(double start_x);
	void	set_start_y(double start_y);
	void	set_tile_width(double tile_width);
	void	set_tile_height(double tile_height);

	double	get_alpha() const;
	double	get_red_intensity() const;
	double	get_green_intensity() const;
	double	get_blue_intensity() const;

	virtual void	set_alpha(double alpha);
	virtual void	set_red_intensity(double r);
	virtual void	set_green_intensity(double g);
	virtual void	set_blue_intensity(double b);

	void	draw(const GameWindow* window) const;
};

#endif
