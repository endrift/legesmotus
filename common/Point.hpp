/*
 * common/Point.hpp
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

#ifndef LM_COMMON_POINT_HPP
#define LM_COMMON_POINT_HPP

#include <iosfwd>
#include <limits>

// A simple class which represents a point or a vector in the Cartesian plane.
// Has x and y components.
namespace LM {
	class Point {
	public:
		float x;
		float y;
	
		Point() { clear(); }
		Point(float arg_x, float arg_y) { x = arg_x; y = arg_y; }
	
		void		clear() { x = y = 0; }
		void		scale(float factor);
		void		rotate(float angle);	// Rotate CW given number of _radians_
		float		get_angle() const;	// In radians
		float		get_magnitude() const;
		Point		get_unit_vector() const;
		bool		is_valid() const { return (x == x && y == y); };
		
		static Point	get_invalid_point() { return Point(std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::quiet_NaN()); };
	
		// Given a string of the from "x,y", make a point from it
		static Point	make_from_string(const char* str);
		// Given a magnitude and an angle (in radians), make a point from it
		static Point	make_from_magnitude(float magnitude, float angle);

		inline static float dot_product(Point u, Point v) {
			return u.x * v.x + u.y * v.y;
		}
		inline static float cross_product(Point u, Point v) {
			return u.x * v.y - u.y * v.x;
		}

		void		operator+=(Point other);
		void		operator-=(Point other);
		void		operator*=(float scale_factor);
		void		operator/=(float scale_factor);
	
		static float distance(Point a, Point b);
	};
	
	// Output in the form "x,y"
	std::ostream& operator<< (std::ostream& out, Point point);
	
	// Arithmetic
	Point	operator+(Point a, Point b);
	Point	operator-(Point a, Point b);
	Point	operator*(Point p, float scale_factor);
	Point	operator/(Point p, float scale_factor);

	// Comparison
	inline bool	operator==(Point a, Point b) { return a.x == b.x && a.y == b.y; }
	inline bool	operator!=(Point a, Point b) { return a.x != b.x || a.y != b.y; }
	
	typedef Point Vector;
}

#endif
