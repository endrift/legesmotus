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

double	Polygon::intersects_circle(Point p, double radius) const {
	list<pair<Point, Point> >::const_iterator it;
	for ( it=m_lines.begin() ; it != m_lines.end(); it++ ) {
		int x1 = it->first.x;
		int y1 = it->first.y;
		int x2 = it->second.x;
		int y2 = it->second.y;
		double dtoline = fabs(double((x2-x1)*(y1-p.y) - (x1-p.x)*(y2-y1) ) / sqrt(double((x2-x1)*(x2-x1) + (y2-y1)*(y2-y1))));
		double partofline = ((p.x - x1) * (x2 - x1) + (p.y - y1) * (y2 - y1)) / fabs(double(((x2-x1) * (x2-x1)) + ((y2-y1) * (y2-y1))));
		if (dtoline < radius && partofline > 0 && partofline < 1) {
			return dtoline;
		}
		
		double dtocorner = sqrt(double((x1-p.x) * (x1-p.x) + (y1-p.y) * (y1-p.y)));
		if (dtocorner < radius) {
			return dtocorner;
		}
		
		dtocorner = sqrt(double((x2-p.x) * (x2-p.x) + (y2-p.y) * (y2-p.y)));
		if (dtocorner < radius) {
			return dtocorner;
		}
	}
	return -1;
}
