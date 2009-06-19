/*
 * client/TiledGraphic.hpp
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
	explicit TiledGraphic(const Graphic& texture);
	TiledGraphic(const TiledGraphic& other);
	virtual TiledGraphic* clone() const;

	double	get_width() const;
	double	get_height() const;
	double	get_start_x() const;
	double	get_start_y() const;
	double	get_tile_width() const;
	double	get_tile_height() const;

	virtual void	set_width(double width);
	virtual void	set_height(double height);
	void	set_start_x(double start_x); // XXX Deprecated
	void	set_start_y(double start_y); // XXX Deprecated
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
