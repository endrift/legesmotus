/*
 * common/Polygon.hpp
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

#ifndef LM_COMMON_POLYGON_HPP
#define LM_COMMON_POLYGON_HPP

#include "common/Point.hpp"
#include "common/Shape.hpp"
#include <list>
#include <utility>

// A polygon is collection of lines which should all join together to create a polygon
namespace LM {
	class Polygon : public Shape {
	private:
		std::list<std::pair<Point, Point> >	m_lines;
	
	public:
	
		// Add a line from point a to point b:
		void			add_line(Point a, Point b);
		void			clear() { m_lines.clear(); }
	
		// Set the polygon to a rectangle of the given width and height
		void			make_rectangle(double width, double height);
		void			make_rectangle(double width, double height, Point upper_left); // make the rectangle starting at given upper left corner


		virtual double		boundary_intersects_circle(const Circle& circle, double* angle) const;
		virtual double		solid_intersects_circle(const Circle& circle, double* angle) const;
		virtual Point		intersects_line(Point start, Point end) const;
	

		// XXX: depreciated function - use boundary_intersects_circle instead
		double			intersects_circle(Point point, double radius, double* angle) const;
		
		// Return the distance of this polygon from the given circle, 
		double			dist_from_circle(Point point, double radius) const;


		// Are there any lines in this polygon?
		// An empty polygon would be used (for example) in a non-intersectable map object (i.e. a decoration or background)
		bool			is_empty() const { return m_lines.empty(); }
		bool			is_filled() const { return !m_lines.empty(); }
	
	};
}

#endif
