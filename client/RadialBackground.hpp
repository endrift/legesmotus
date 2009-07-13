/*
 * client/RadialBackground.hpp
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

#ifndef LM_CLIENT_RADIALBACKGROUND_HPP
#define LM_CLIENT_RADIALBACKGROUND_HPP

#include "Graphic.hpp"
#include <vector>

namespace LM {
	class RadialBackground : public Graphic {
	private:
		static const int RESOLUTION;

		int	m_num_segments;
		std::vector<Color>	m_segment_colors;

		Color	m_border_color;
		double	m_border_radius;
		double	m_border_angle;
		double	m_outer_radius;
		double	m_inner_radius;

	public:
		RadialBackground(int num_segments);
		virtual RadialBackground* clone() const;

		int	get_num_segments() const;
	
		void	set_num_segments(int num_rows);
		void	set_border_radius(double radius);
		void	set_border_angle(double degrees);
		void	set_border_color(Color color);
		void	set_segment_color(int segment, Color color);
		void	set_outer_radius(double radius);
		void	set_inner_radius(double radius);

		virtual void	draw(const GameWindow* window) const;
	};
}

#endif
