/*
 * common/math.hpp
 *
 * Copyright 2009 - Nathan Partlan, Andrew Ayer, Daniel Schneider, and Jeffrey Pfau
 * 
 */

#include <stdint.h>
#include <cmath>

#ifndef M_PI
#define M_PI 3.141592653589793
#endif

const double RADIANS_TO_DEGREES = 57.29577951308232;
const double DEGREES_TO_RADIANS = 1.745329251994e-02;

static uint32_t toPow2(uint32_t num) {
	--num;
	for (int i = 1; i < 32; i <<= 1) {
		num |= num >> i;
	}
	++num;
	return num;
}
