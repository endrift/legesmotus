/*
 * common/math.hpp
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

#ifndef LM_COMMON_MATH_H
#define LM_COMMON_MATH_H

#include <stdint.h>
#include <cmath>
#include <vector>

#ifndef M_PI
#define M_PI (3.141592653589793)
#endif

#ifndef M_E
#define M_E (2.718281828459045)
#endif

namespace LM {
	// For easy conversion between radians and degrees:
	extern const double RADIANS_TO_DEGREES;
	extern const double DEGREES_TO_RADIANS;

	inline uint32_t toPow2(uint32_t num) {
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

	// Go from degrees to radians
	inline double to_radians(double degrees) { return degrees * DEGREES_TO_RADIANS; }

	// Go from radians to degrees
	inline double to_degrees(double radians) { return radians * RADIANS_TO_DEGREES; }
}

#endif
