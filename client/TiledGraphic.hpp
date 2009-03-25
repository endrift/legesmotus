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

	void	init();

public:
	TiledGraphic(SDL_Surface* image);
	TiledGraphic(const char* filename);
	TiledGraphic(const TiledGraphic& other);

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

	void	draw(const GameWindow* window) const;
};

#endif
