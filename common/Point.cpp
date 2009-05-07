#include "common/Point.hpp"
#include <string.h>
#include <stdlib.h>
#include <ostream>

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

ostream&	operator<<(ostream& out, Point point) {
	return out << point.x << ',' << point.y;
}

Point	operator+(Point a, Point b) {
	return Point(a.x + b.x, a.y + b.y);
}
Point	operator-(Point a, Point b) {
	return Point(a.x - b.x, a.y - b.y);
}
