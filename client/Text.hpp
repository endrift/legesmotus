/*
 * client/Text.hpp
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

#ifndef LM_CLIENT_TEXT_HPP
#define LM_CLIENT_TEXT_HPP

#include <string>
#include "Font.hpp"
#include "Graphic.hpp"
#include "Sprite.hpp"
#include "ConvolveKernel.hpp"

namespace LM {
	class Text : public Graphic {
	private:
		Sprite* m_fg;
		Sprite* m_shadow;
		bool	m_shadow_enabled;
	
		void	touch_shadow(); // Makes sure the shadow exists
	public:
		Text(const std::string& text, Font* font, const ConvolveKernel* convolve = NULL);
		Text(const Text& other);
		virtual ~Text();
		virtual Text* clone() const;
	
		void		set_color(double r, double g, double b); // Only sets FG color
		void		set_color(const Color& color);
		virtual void	set_alpha(double alpha); // Only sets FG alpha
		void		set_shadow_color(double r, double g, double b);
		void		set_shadow_color(const Color& color);
		void		set_shadow_alpha(double a);
		void		set_shadow_offset(double x, double y);
		void		set_shadow(bool enable);
	
		virtual void	set_red_intensity(double r);
		virtual void	set_green_intensity(double g);
		virtual void	set_blue_intensity(double b);
	
		virtual void	draw(const GameWindow* window) const;
	};
}

#endif
