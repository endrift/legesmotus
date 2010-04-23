/*
 * common/Circle.cpp
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

#include "common/Circle.hpp"
#include "common/math.hpp"
#include <cmath>

using namespace LM;
using namespace std;

Circle::Circle (Point arg_center, double arg_radius) : center(arg_center), radius(arg_radius) {
}

double	Circle::get_circumference() const {
	return M_PI * radius * 2.0;
}

double	Circle::get_area() const {
	return M_PI * radius * radius;
}

double	Circle::boundary_intersects_circle(const Circle& other, double* angle) const {
	double distance = Point::distance(center, other.center);
	if (distance >= fabs(radius - other.radius) && distance <= radius + other.radius) {
		if (angle) {
			*angle = to_degrees(get_angle_of_incidence(other));
		}
		return distance - fabs(radius - other.radius);
	}
	return -1;
}

double	Circle::solid_intersects_circle(const Circle& other, double* angle) const {
	double distance = Point::distance(center, other.center);
	if (distance <= radius + other.radius) {
		if (distance >= fabs(radius - other.radius)) {
			if (angle) {
				*angle = to_degrees(get_angle_of_incidence(other));
			}
			return distance - fabs(radius - other.radius);
		} else {
			// One circle fully inside the other
			return 0;
		}
	}
	return -1;
}

Point	Circle::intersects_line(Point start, Point end, double* angle) const {
	Point	closest = closest_point_on_line_to_point(start, end, center);
	if (closest.x == -1 && closest.y == -1) {
		// This circle's not near the line
		return Point(-1, -1);
	}

	double	closest_distance = Point::distance(center, closest);
	if (closest_distance > radius) {
		// The line is too far away from the circle
		return Point(-1, -1);
	} else if (closest_distance == radius) {
		// The line is tangent to the circle
		return closest;
	}

	// Use Pythagorean Theorem to determine how far the intersection points are from the closest point
	double	intersection_distance = sqrt(radius * radius - closest_distance * closest_distance);

	// Find the distance from the start point to an intersection point
	double	distance_from_start = Point::distance(start, closest) - intersection_distance;

	Point	point_of_intersection(start + Vector::make_from_magnitude(distance_from_start, (end - start).get_angle()));

	if (angle != NULL) {
		*angle = to_degrees(get_angle_of_incidence(point_of_intersection));
	}
	return point_of_intersection;
}

double	Circle::get_angle_of_incidence (const Circle& other_circle) const {
	return Vector(center - other_circle.center).get_angle();
}

double	Circle::get_angle_of_incidence (Point point_on_circle) const {
	return Vector(center - point_on_circle).get_angle();
}

void	Circle::rotate (double angle) {
	// Rotating a circle is easy :-)
}

void	Circle::scale (double factor) {
	radius *= factor;
}

double	Circle::dist_from_circle(const Circle& other) const {
	double distance = Point::distance(center, other.center);
	if (distance > radius + other.radius) {
		return distance - (radius + other.radius);
	} else {
		return 0.0;
	}
}

