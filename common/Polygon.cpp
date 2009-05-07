/*
 * common/Polygon.cpp
 *
 * Copyright 2009 - Nathan Partlan, Andrew Ayer, Daniel Schneider, and Jeffrey Pfau
 * 
 */

#include "Polygon.hpp"
#include <math.h>
#include <cmath>
#include <iostream>
#include <limits>
#include "common/math.hpp"

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

double	Polygon::intersects_circle(Point p, double radius, double* angle) const {
	list<pair<Point, Point> >::const_iterator it;
	double min_dist = numeric_limits<double>::max();
	double x1res;
	double y1res;
	double x2res;
	double y2res;
	for ( it=m_lines.begin() ; it != m_lines.end(); it++ ) {
		int x1 = it->first.x;
		int y1 = it->first.y;
		int x2 = it->second.x;
		int y2 = it->second.y;
		double dtoline = dist_from_line_to_point(x1, y1, x2, y2, p.x, p.y);
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

Point Polygon::intersects_line(Point start, Point end) const {
	double mindist = numeric_limits<double>::max();
	
	Point p = start;
	Point r = end - start;
	list<pair<Point, Point> >::const_iterator it;
	for ( it=m_lines.begin() ; it != m_lines.end(); it++ ) {
		Point q = it->first;
		Point s = it->second - it->first;
		Point zeroedstart = q - p;
		double t = cross_product(zeroedstart, s) / cross_product(r, s);
		double u = cross_product(zeroedstart, r) / cross_product(r, s);
		
		if (t >= 0 && t <= 1 && u >= 0 && u <= 1) {
			if (t < mindist) {
				mindist = t;
			}
		}
	}
	
	if (mindist != numeric_limits<double>::max()) {
		Point scale = Point(mindist * r.x, mindist * r.y);
		return start + scale;
	}
	return Point(-1, -1);
}

double Polygon::cross_product(Point start, Point end) const {
	return start.x * end.y - start.y * end.x;
}

double Polygon::dist_from_line_to_point(double x1, double y1, double x2, double y2, double px, double py) const {
	return fabs(double((x2-x1)*(y1-py) - (x1-px)*(y2-y1) ) / sqrt(double((x2-x1)*(x2-x1) + (y2-y1)*(y2-y1))));
}

Point Polygon::closest_point_on_line_to_point(Point start, Point end, Point p) const {
	Point v = end - start;
	Point w = p - start;
	double projection = dot_product(v, w) / dot_product(v, v);
	if (projection < 0 || projection > 1) {
		return Point(-1, -1);
	} else {
		return start + Point(v.x * projection, v.y * projection);
	}
}

double	Polygon::dist_from_circle(Point p, double radius) const {
	list<pair<Point, Point> >::const_iterator it;
	double min_dist = numeric_limits<double>::max();
	for ( it=m_lines.begin() ; it != m_lines.end(); it++ ) {
		int x1 = it->first.x;
		int y1 = it->first.y;
		int x2 = it->second.x;
		int y2 = it->second.y;
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
	return min_dist;
}
