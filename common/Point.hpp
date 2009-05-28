/*
 * common/Point.hpp
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

#ifndef LM_COMMON_POINT_HPP
#define LM_COMMON_POINT_HPP

#include <iosfwd>
#include <math.h>

// A simple class which represents a point in the Cartesian plane.
// Has an x and y component.
class Point {
public:
	int		x;
	int		y;

	Point() { clear(); }
	Point(int arg_x, int arg_y) { x = arg_x; y = arg_y; }
	Point(double arg_x, double arg_y) { x = int(round(arg_x)); y = int(round(arg_y)); }

	// Given a string of the from "x,y" initialize this point to it
	void		init_from_string(const char* str);
	void		clear() { x = y = 0; }

	inline static int dot_product(Point a, Point b) {
		return a.x * b.x + a.y * b.y;
	}

	static double distance(Point a, Point b);
};

// Output in the form "x,y"
std::ostream& operator<< (std::ostream& out, Point point);

// Arithmetic
Point	operator+(Point a, Point b);
Point	operator-(Point a, Point b);


#endif
