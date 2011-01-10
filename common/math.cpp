/*
 * common/math.cpp
 *
 * This file is part of Leges Motus, a networked, 2D shooter set in zero gravity.
 * 
 * Copyright 2009-2010 Andrew Ayer, Nathan Partlan, Jeffrey Pfau
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

#include "math.hpp"
#include "misc.hpp"
#include "Point.hpp"
#include <iostream>

// See .hpp file for extensive comments.

using namespace LM;
using namespace std;

const double LM::RADIANS_TO_DEGREES = 57.29577951308232;
const double LM::DEGREES_TO_RADIANS = 1.745329251994e-02;

double LM::get_normalized_angle(double angle) {
	while (angle < 0) {
		angle += 360;
	}
	while (angle >= 360) {
		angle -= 360;
	}
	return angle;
}

Point LM::closest_point_on_line_to_point(Point start, Point end, Point p, bool is_segment) {
	Point v = end - start;
	Point w = p - start;
	double projection = Point::dot_product(v, w) / Point::dot_product(v, v);
	if (is_segment && (projection < 0 || projection > 1)) {
		return Point(-1, -1);
	} else {
		return start + Point(v.x * projection, v.y * projection);
	}
}

double LM::dist_from_line_to_point(Point start, Point end, Point p) {
	const double x1 = start.x;
	const double y1 = start.y;
	const double x2 = end.x;
	const double y2 = end.y;
	const double px = p.x;
	const double py = p.y;

	return fabs(double((x2-x1)*(y1-py) - (x1-px)*(y2-y1) ) / sqrt(double((x2-x1)*(x2-x1) + (y2-y1)*(y2-y1))));
}

