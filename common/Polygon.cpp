/*
 * common/Polygon.cpp
 *
 * Copyright 2009 - Nathan Partlan, Andrew Ayer, Daniel Schneider, and Jeffrey Pfau
 * 
 */

#include "Polygon.hpp"

void	Polygon::add_line(Point a, Point b) {
	m_lines.push_back(make_pair(a, b));
}

bool	Polygon::contains(Point a) const { // TODO
	return false;
}
