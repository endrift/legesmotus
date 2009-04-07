/*
 * common/Point.hpp
 *
 * Copyright 2009 - Nathan Partlan, Andrew Ayer, Daniel Schneider, and Jeffrey Pfau
 * 
 */

#ifndef LM_COMMON_POINT_HPP
#define LM_COMMON_POINT_HPP

#include <iosfwd>

class Point {
public:
	int		x;
	int		y;

	Point() { clear(); }
	Point(int arg_x, int arg_y) { x = arg_x; y = arg_y; }

	void		init_from_string(const char* str); // str to be of form x,y
	void		clear() { x = y = 0; }
};

std::ostream& operator<< (std::ostream& out, Point point);

#endif
