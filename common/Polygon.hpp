/*
 * common/Polygon.hpp
 *
 * Copyright 2009 - Nathan Partlan, Andrew Ayer, Daniel Schneider, and Jeffrey Pfau
 * 
 */

#ifndef LM_COMMON_POLYGON_HPP
#define LM_COMMON_POLYGON_HPP

#include <list>
#include <utility>
#include "common/Point.hpp"

using namespace std;

class Polygon {
private:
	list<pair<Point, Point> >	m_lines;

public:

	void			add_line(Point a, Point b);

	// Return true if this bounding polygon contains the point
	bool			contains(Point point) const;

};

#endif
