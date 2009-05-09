/*
 * common/math.hpp
 *
 * Copyright 2009 - Nathan Partlan, Andrew Ayer, Daniel Schneider, and Jeffrey Pfau
 * 
 */

#ifndef LM_COMMON_MATH_H
#define LM_COMMON_MATH_H

#include <stdint.h>
#include <cmath>
#include <vector>
#include "common/Point.hpp"

#ifndef M_PI
#define M_PI 3.141592653589793
#endif

// For easy conversion between radians and degrees:
extern const double RADIANS_TO_DEGREES;
extern const double DEGREES_TO_RADIANS;

static inline uint32_t toPow2(uint32_t num) {
	--num;
	for (int i = 1; i < 32; i <<= 1) {
		num |= num >> i;
	}
	++num;
	return num;
}

std::vector<double> closest_point_on_line(double x1, double y1, double x2, double y2, double x3, double y3);

// Get a value in the range [0,360)
double get_normalized_angle(double angle);

double dot_product(Point a, Point b);

#endif
