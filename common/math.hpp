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

using namespace std;

#ifndef M_PI
#define M_PI 3.141592653589793
#endif

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

vector<int> closest_point_on_line(int x1, int y1, int x2, int y2, int x3, int y3);

static inline double dist_between_points(int x1, int y1, int x2, int y2) {
	return sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}

#endif
