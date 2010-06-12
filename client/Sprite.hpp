/*
 * client/Sprite.hpp
 *
 * This file is part of Leges Motus, a networked, 2D shooter set in zero gravity.
 * 
 * Copyright 2009-2010 Andrew Ayer, Nathan Partlan, Jeffrey Pfau
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

#ifndef LM_CLIENT_SPRITE_HPP
#define LM_CLIENT_SPRITE_HPP

#include "Graphic.hpp"

namespace LM {
	class Sprite : public Graphic {
	private:
		int		m_width;
		int		m_height;
	
		double		m_alpha;
		double		m_red;
		double		m_green;
		double		m_blue;
	
		void		init();
	
	public:
		Sprite(SDL_Surface* image);
		Sprite(const char* filename);
		explicit Sprite(const Graphic& texture);
		Sprite(const Sprite& other);
		virtual Sprite* clone() const;
	
		int	get_width() const;
		int	get_height() const;
	
		double	get_alpha() const;
		double	get_red_intensity() const;
		double	get_green_intensity() const;
		double	get_blue_intensity() const;
		Color	get_color_intensity() const;
	
		virtual void	set_alpha(double alpha);
		virtual void	set_red_intensity(double r);
		virtual void	set_green_intensity(double g);
		virtual void	set_blue_intensity(double b);
	
		void	set_antialiasing(bool enable);
	
		void	draw() const;
	};
}

#endif
