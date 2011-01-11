/*
 * common/Shape.hpp
 *
 * This file is part of Leges Motus, a networked, 2D shooter set in zero gravity.
 * 
 * Copyright 2009-2011 Andrew Ayer, Nathan Partlan, Jeffrey Pfau
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
		// Also stores the angle of incidence (degrees) in angle, if not NULL.
		// This is the angle of the intersecting surface, and has nothing to do with the angle between the line and the shape
		virtual Point		intersects_line(Point start, Point end, double* angle) const = 0;


		// Return the distance to this shape from the given circle
		virtual double		dist_from_circle(const Circle& circle) const = 0;


		// Rotate the shape CW by the given angle (in degrees)
		virtual void		rotate (double angle) = 0;

		// Scale the shape by the given factor
		virtual void		scale (double factor) = 0;
		
		// Move the shape by the given factor
		virtual void		translate (double x, double y) = 0;

		
		// Returns true if this shape is oriented from its center (e.g. a circle),
		//  false if the shape is oriented from its top left (e.g. a polygon)
		virtual bool		is_centered () const = 0;


		// Given a string describing a shape, construct the shape and return it:
		// Construct the shape relative to the given position
		// WARNING: Returns pointer to the free store (your responsibility to delete!)
		// Format of recognized strings:
		//  poly:x1,y1;x2,y2;x3,y3;...
		//  circle:radius
		//  rect:width,height
		static Shape*		make_from_string(const char* str, Point position);
	};
}

#endif
