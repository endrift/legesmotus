/*
 * common/math.cpp
 *
 * Copyright 2009 - Nathan Partlan, Andrew Ayer, Daniel Schneider, and Jeffrey Pfau
 * 
 */

#include "math.hpp"
#include <iostream>

const double RADIANS_TO_DEGREES = 57.29577951308232;
const double DEGREES_TO_RADIANS = 1.745329251994e-02;

vector<int> closest_point_on_line(int x1, int y1, int x2, int y2, int x3, int y3) {
	double distp2p1 = dist_between_points(x1, y1, x2, y2);
	if (distp2p1 == 0) {
		cerr << "Error: Distance between start and end of line is 0.";
		return vector<int>();
	}
	double utop = ((x3 - x1) * (x2 - x1) + (y3 - y1) * (y2 - y1));
	double u = utop / (distp2p1 * distp2p1);
	int xclosest = x1 + u * (x2 - x1);
	int yclosest = y1 + u * (y2 - y1);
	vector<int> closestpoint;
	closestpoint.push_back(xclosest);
	closestpoint.push_back(yclosest);
	return closestpoint;
}
