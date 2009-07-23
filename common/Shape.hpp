/*
 * common/Shape.hpp
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

#ifndef LM_COMMON_SHAPE_HPP
#define LM_COMMON_SHAPE_HPP

#include "common/Point.hpp"

namespace LM {
	class Circle;

	class Shape {
	public:
		virtual ~Shape () { }

		// Return the distance if this shape's _boundary_ intersects the given circle.
		// Return -1 if not intersecting.
		// Also stores the angle of incidence (degrees) in angle, if not NULL.
		virtual double		boundary_intersects_circle(const Circle& circle, double* angle) const = 0;


		// Like boundary_intersects_circle, but also check if the interior of the shape intersects with the given circle.
		// Returns 0 if the circle lies fully inside the shape
		// Returns -1 if not intersecting
		// Also stores the angle of incidence (degrees) in angle, if not NULL.
		virtual double		solid_intersects_circle(const Circle& circle, double* angle) const = 0;


		// Return the point of intersection if this shape intersects with the (finite) line.
		// Otherwise return Point(-1, -1).
		virtual Point		intersects_line(Point start, Point end) const = 0;
	};
}

#endif
