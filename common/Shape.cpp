/*
 * common/Shape.cpp
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

#include "Shape.hpp"
#include "Polygon.hpp"
#include "Circle.hpp"
#include "StringTokenizer.hpp"
#include <cstring>
#include <cstdlib>
#include <memory>

using namespace LM;
using namespace std;

Shape*	Shape::make_from_string(const char* str, Point position) {
	if (strncmp(str, "poly:", 5) == 0) {
		auto_ptr<Polygon>	poly(new Polygon(position));

		// Polygon - specified by a list of points.
		// The points are converted into a list of lines for internal representation.
		StringTokenizer		tokenizer(str + 5, ';');

		Point			first_point;
		tokenizer >> first_point;

		Point			previous_point(first_point);
		while (tokenizer.has_more()) {
			Point		next_point;
			tokenizer >> next_point;
			poly->add_line(previous_point, next_point);

			previous_point = next_point;
		}
		poly->add_line(previous_point, first_point);

		return poly.release();
	} else if (strncmp(str, "rect:", 5) == 0) {
		Vector			size(Vector::make_from_string(str + 5));
		auto_ptr<Polygon>	poly(new Polygon(position));
		poly->make_rectangle(size.x, size.y);

		return poly.release();
	} else if (strncmp(str, "circle:", 7) == 0) {
		return new Circle(position, atof(str + 7));
	}

	return NULL;
}

