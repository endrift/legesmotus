/*
 * common/Point.cpp
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

#include "common/Point.hpp"
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ostream>

using namespace LM;
using namespace std;

// See .hpp file for extensive comments.

void	Point::init_from_string(const char* str) {
	// Look for a comma
	if (const char* comma = strchr(str, ',')) {
		x = atoi(str);		// x starts from beginning of string
		y = atoi(comma + 1);	// y starts after the comma
	} else {
		// No comma found.
		clear();
	}
}

double	Point::distance(Point a, Point b) {
	return sqrt(double((b.x - a.x) * (b.x - a.x) + (b.y - a.y) * (b.y - a.y)));
}

ostream&	LM::operator<<(ostream& out, Point point) {
	return out << point.x << ',' << point.y;
}

Point	LM::operator+(Point a, Point b) {
	return Point(a.x + b.x, a.y + b.y);
}
Point	LM::operator-(Point a, Point b) {
	return Point(a.x - b.x, a.y - b.y);
}
