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
		typedef std::list<std::pair<Point, Point> > LineList;

		Point			m_upper_left;
		LineList		m_lines;
		// Note: all lines are stored ABSOLUTELY from the origin
		// (and not relatively from m_upper_left)
		// m_upper_left is only kept so that scaling and rotating work
	
	public:
		Polygon() { }
		explicit Polygon(Point upper_left);

		// Add a line from point a to point b:
		// The line will be added relative to m_upper_left
		void			add_line(Point a, Point b);
		void			clear() { m_lines.clear(); }
	
		// Set the polygon to a rectangle of the given width and height
		// The rectangle will be made relative to m_upper_left
		void			make_rectangle(double width, double height);


		virtual double		boundary_intersects_circle(const Circle& circle, double* angle) const;
		virtual double		solid_intersects_circle(const Circle& circle, double* angle) const;
		virtual Point		intersects_line(Point start, Point end) const;
	
		virtual void		rotate (double angle);
		virtual void		scale (double factor);
	
		virtual bool		is_centered () const { return false; }

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
