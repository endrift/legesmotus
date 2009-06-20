/*
 * client/TableBackground.hpp
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

#ifndef LM_CLIENT_TABLEBACKGROUND_HPP
#define LM_CLIENT_TABLEBACKGROUND_HPP

#include "Graphic.hpp"
#include <vector>

namespace LM {
	class TableBackground : public Graphic {
	private:
		int	m_num_rows;
		std::vector<Color>	m_cell_colors;
		std::vector<double>	m_row_heights;
	
		Color	m_border_color;
		double	m_border_width;
		double	m_corner_radius;
		bool	m_border_collapse;
	
		void draw_row(int row) const;
	public:
		TableBackground(int num_rows, double width);
		virtual TableBackground* clone() const;
	
		int	get_num_rows() const;
		double	get_row_height(int row) const;
	
		void	set_num_rows(int num_rows);
		void	set_row_height(int row, double height);
		void	set_image_width(double width);
	
		void	set_border_width(double pixels);
		void	set_border_color(Color color);
		void	set_cell_color(int row, Color color);
		void	set_corner_radius(double radius);
		void	set_border_collapse(bool collapse);
	
		virtual void	set_alpha(double alpha) { (void)(alpha); }
		virtual void	set_red_intensity(double r) { (void)(r); }
		virtual void	set_green_intensity(double g) { (void)(g); }
		virtual void	set_blue_intensity(double b) { (void)(b); }
	
		virtual void	set_width(double width) { set_image_width(width); }
		virtual void	set_height(double height);
	
		virtual void	draw(const GameWindow* window) const;
	};
	
}

#endif
