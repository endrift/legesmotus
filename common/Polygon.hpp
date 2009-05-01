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

// A polygon is collection of lines which should all join together to create a polygon
class Polygon {
private:
	std::list<std::pair<Point, Point> >	m_lines;

public:

	void			add_line(Point a, Point b);
	void			clear() { m_lines.clear(); }

	// Set the polygon to a rectangle of the given width and height
	void			make_rectangle(int width, int height);
	void			make_rectangle(int width, int height, Point upper_left); // make the rectangle starting at given upper left corner

	// Return the distance if this bounding polygon intersects with the circle. Otherwise returns -1.
	double			intersects_circle(Point point, double radius) const;
	
	// Return the point of intersection if the bounding polygon intersects with the line. Otherwise return -1.
	Point			intersects_line(Point start, Point end) const;
	
	double			cross_product(Point start, Point end) const;
	
	double			dist_from_circle(Point point, double radius) const;

	// Are there any lines in this polygon?
	// An empty polygon would be used (for example) in a non-intersectable map object (i.e. a decoration or background)
	bool			is_empty() const { return m_lines.empty(); }
	bool			is_filled() const { return !m_lines.empty(); }

};

#endif
