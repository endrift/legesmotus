/*
 * common/Point.hpp
 *
 * Copyright 2009 - Nathan Partlan, Andrew Ayer, Daniel Schneider, and Jeffrey Pfau
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
