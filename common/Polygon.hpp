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

class Polygon {
private:
	std::list<std::pair<Point, Point> >	m_lines;

public:

	void			add_line(Point a, Point b);
	void			clear() { m_lines.clear(); }

	// Set the polygon to a rectangle of the given width and height
	void			make_rectangle(int width, int height);

	// Return true if this bounding polygon contains the point
	bool			contains(Point point) const;

	bool			is_empty() const { return m_lines.empty(); }
	bool			is_filled() const { return !m_lines.empty(); }

};

#endif
