/*
 * common/Polygon.cpp
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

#include "Polygon.hpp"
#include <math.h>
#include <cmath>
#include <iostream>
#include <limits>
#include "common/math.hpp"
#include "common/Circle.hpp"

// See .hpp file for extensive comments.

using namespace LM;
using namespace std;

Polygon::Polygon(Point upper_left) : m_upper_left(upper_left) {
}

void	Polygon::add_line(Point a, Point b) {
	m_lines.push_back(make_pair(m_upper_left + a, m_upper_left + b));
}

void	Polygon::make_rectangle(double width, double height) {
	clear();
	add_line(Point(0, 0), Point(width, 0));
	add_line(Point(width, 0), Point(width, height));
	add_line(Point(width, height), Point(0, height));
	add_line(Point(0, height), Point(0, 0));
}

double	Polygon::boundary_intersects_circle(const Circle& circle, double* angle) const {
	const Point&	p(circle.center);
	const double	radius(circle.radius);

	double min_dist = numeric_limits<double>::max();
	double x1res = 0;
	double y1res = 0;
	double x2res = 0;
	double y2res = 0;
	for (LineList::const_iterator it(m_lines.begin()); it != m_lines.end(); ++it) {
		const double x1 = it->first.x;
		const double y1 = it->first.y;
		const double x2 = it->second.x;
		const double y2 = it->second.y;

		double dtoline = dist_from_line_to_point(it->first, it->second, p);
		double partofline = ((p.x - x1) * (x2 - x1) + (p.y - y1) * (y2 - y1)) / fabs(double(((x2-x1) * (x2-x1)) + ((y2-y1) * (y2-y1))));
		if (dtoline < radius && partofline > 0 && partofline < 1 && dtoline < min_dist) {
			min_dist = dtoline;
			if (angle != NULL) {
				Point closest_point = closest_point_on_line_to_point(it->first, it->second, p);
				x2res = closest_point.x;
				y2res = closest_point.y;
				
				if (x2res == -1 && y2res == -1) {
					cerr << "x2res and y2res are -1 - could not find closest point on line" << endl;
				}
				
				x1res = p.x;
				y1res = p.y;
			}
		}
		
		double dtocorner = sqrt(double((x1-p.x) * (x1-p.x) + (y1-p.y) * (y1-p.y)));
		if (dtocorner < radius && dtocorner < min_dist) {
			min_dist = dtocorner;
			if (angle != NULL) {
				x1res = p.x;
				y1res = p.y;
				x2res = x1;
				y2res = y1;
			}
		}
		
		dtocorner = sqrt(double((x2-p.x) * (x2-p.x) + (y2-p.y) * (y2-p.y)));
		if (dtocorner < radius && dtocorner < min_dist) {
			min_dist = dtocorner;
			if (angle != NULL) {
				x1res = p.x;
				y1res = p.y;
				x2res = x2;
				y2res = y2;
			}
		}
	}
	
	if (min_dist != numeric_limits<double>::max()) {
		if (angle != NULL) {
			*angle = atan2((y2res-y1res), (x2res-x1res)) * RADIANS_TO_DEGREES;
		}
		return min_dist;
	}
	return -1;
}

double	Polygon::solid_intersects_circle(const Circle& circle, double* angle) const {
	double	boundary_distance = boundary_intersects_circle(circle, angle);
	if (boundary_distance != -1) {
		// An intersection was found with the boundary
		return boundary_distance;
	}

	// Check to see if the center of the circle lies within the polygon.
	// If the circle isn't intersecting the boundary (as determined by check above), but the center of the circle is inside the polygon, then the circle must be wholly inside the polygon.
	// If the center of the circle is outside the polygon, but part of the circle is inside the polygon, then it's intersecting the boundary and the check above already reported an intersection.
	bool		inside = false;
	for (LineList::const_iterator line(m_lines.begin()); line != m_lines.end(); ++line) {
		if ((line->second.y > circle.center.y) != (line->first.y > circle.center.y) &&
			circle.center.x < (line->first.x - line->second.x) * (circle.center.y - line->second.y) / (line->first.y - line->second.y) + line->second.x) {
			inside = !inside;
		}
	}

	return inside ? 0 : -1;
}

Point Polygon::intersects_line(Point start, Point end, double* angle) const {
	double mindist = numeric_limits<double>::max();
	
	Point p = start;
	Point r = end - start;
	LineList::const_iterator	intersecting_side(m_lines.end());
	for (LineList::const_iterator it(m_lines.begin()); it != m_lines.end(); ++it) {
		Point q = it->first;
		Point s = it->second - it->first;
		Point zeroedstart = q - p;
		double t = Point::cross_product(zeroedstart, s) / Point::cross_product(r, s);
		double u = Point::cross_product(zeroedstart, r) / Point::cross_product(r, s);
		
		if (t >= 0 && t <= 1 && u >= 0 && u <= 1) {
			if (t < mindist) {
				mindist = t;
				intersecting_side = it;
			}
		}
	}
	
	if (mindist != numeric_limits<double>::max()) {
		if (angle != NULL) {
			Point closest_point = closest_point_on_line_to_point(intersecting_side->first, intersecting_side->second, start, false);
			*angle = to_degrees((closest_point - start).get_angle());
		}
		Point scale = Point(mindist * r.x, mindist * r.y);
		return start + scale;
	}
	return Point(-1, -1);
}

double	Polygon::dist_from_circle(const Circle& circle) const {
	const Point p(circle.center);

	double min_dist = numeric_limits<double>::max();
	for (LineList::const_iterator it(m_lines.begin()); it != m_lines.end(); ++it) {
		double x1 = it->first.x;
		double y1 = it->first.y;
		double x2 = it->second.x;
		double y2 = it->second.y;
		double dtoline = fabs(double((x2-x1)*(y1-p.y) - (x1-p.x)*(y2-y1) ) / sqrt(double((x2-x1)*(x2-x1) + (y2-y1)*(y2-y1))));
		double partofline = ((p.x - x1) * (x2 - x1) + (p.y - y1) * (y2 - y1)) / fabs(double(((x2-x1) * (x2-x1)) + ((y2-y1) * (y2-y1))));
		if (dtoline < min_dist && partofline > 0 && partofline < 1) {
			min_dist = dtoline;
		}
		
		double dtocorner = sqrt(double((x1-p.x) * (x1-p.x) + (y1-p.y) * (y1-p.y)));
		if (dtocorner < min_dist) {
			min_dist = dtocorner;
		}
		
		dtocorner = sqrt(double((x2-p.x) * (x2-p.x) + (y2-p.y) * (y2-p.y)));
		if (dtocorner < min_dist) {
			min_dist = dtocorner;
		}
	}
	if (min_dist != numeric_limits<double>::max()) {
		if (min_dist > circle.radius) {
			return min_dist - circle.radius;
		}
		return 0;
	}
	return min_dist;
}

void	Polygon::rotate (double angle) {
	for (LineList::iterator it(m_lines.begin()); it != m_lines.end(); ++it) {
		it->first -= m_upper_left;
		it->first.rotate(to_radians(angle));
		it->first += m_upper_left;
		it->second -= m_upper_left;
		it->second.rotate(to_radians(angle));
		it->second += m_upper_left;
	}
}

void	Polygon::scale (double factor) {
	for (LineList::iterator it(m_lines.begin()); it != m_lines.end(); ++it) {
		it->first -= m_upper_left;
		it->first.scale(factor);
		it->first += m_upper_left;
		it->second -= m_upper_left;
		it->second.scale(factor);
		it->second += m_upper_left;
	}
}

