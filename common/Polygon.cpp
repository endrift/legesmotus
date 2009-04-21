/*
 * common/Polygon.cpp
 *
 * Copyright 2009 - Nathan Partlan, Andrew Ayer, Daniel Schneider, and Jeffrey Pfau
 * 
 */

#include "Polygon.hpp"

using namespace std;

void	Polygon::add_line(Point a, Point b) {
	m_lines.push_back(make_pair(a, b));
}

void	Polygon::make_rectangle(int width, int height) {
	clear();
	add_line(Point(0, 0), Point(width, 0));
	add_line(Point(width, 0), Point(width, height));
	add_line(Point(width, height), Point(0, height));
	add_line(Point(0, height), Point(0, 0));
}

void	Polygon::make_rectangle(int width, int height, Point upper_left) {
	clear();
	add_line(upper_left, upper_left + Point(width, 0));
	add_line(upper_left + Point(width, 0), upper_left + Point(width, height));
	add_line(upper_left + Point(width, height), upper_left + Point(0, height));
	add_line(upper_left + Point(0, height), upper_left);
}

bool	Polygon::contains(Point a) const { // TODO
	return false;
}
