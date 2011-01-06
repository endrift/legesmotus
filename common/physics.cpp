/*
 * common/physics.cpp
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

#include "physics.hpp"
#include "common/StringTokenizer.hpp"
#include <cstring>
#include "common/misc.hpp"

using namespace LM;
using namespace std;

b2Shape* LM::make_shape_from_string(const std::string& shape_string, float scale_x, float scale_y) {
	// TODO: Support scaling of shape!
	const char* str = shape_string.c_str();
	
	if (strncmp(str, "poly:", 5) == 0) {
		b2PolygonShape* shape = new b2PolygonShape();

		// Polygon - specified by a list of points.
		// The points are converted into a list of lines for internal representation.
		// NOTE: Points MUST be in clockwise order.
		StringTokenizer		tokenizer(str + 5, ';');

		std::vector<Point> points;
		Point			first_point;
		
		tokenizer >> first_point;
		points.push_back(first_point);
		
		Point			previous_point(first_point);
		while (tokenizer.has_more()) {
			Point		next_point;
			tokenizer >> next_point;
			if ((next_point.x == previous_point.x && next_point.y == previous_point.y) || 
				(next_point.x == first_point.x && next_point.y == first_point.y)) {
				continue;
			}
			points.push_back(next_point);
			previous_point = next_point;
		}
		
		if (points.size() < 3) {
			WARN("Error: tried to make a polygon with fewer than three points.");
			return NULL;
		}
		
		// We must ensure that the points are in clockwise order, so that we can avoid errors in Box2D.
		Point a = points.at(0);
		Point b = points.at(1);
		Point c = points.at(2);
		float area = a.x * b.y - a.y * b.x + a.y * c.x - a.x * c.y + b.x * c.y - c.x * b.y;
		
		b2Vec2 vertices[points.size()];
		if (area <= 0) {
			for (int i = points.size()-1; i >= 0; i--) {
				vertices[points.size()-1 - i].Set(to_physics(points.at(i).x), to_physics(points.at(i).y));
			}
		} else {
			for (unsigned int i = 0; i < points.size(); i++) {
				vertices[i].Set(to_physics(points.at(i).x), to_physics(points.at(i).y));
			}
		}
		
		shape->Set(vertices, points.size());

		return shape;
	} else if (strncmp(str, "rect:", 5) == 0) {
		Vector size(Vector::make_from_string(str + 5));
		b2PolygonShape* shape = new b2PolygonShape();
		
		shape->SetAsBox(to_physics(size.x/2 * scale_x), to_physics(size.y/2 * scale_y),
		                b2Vec2(to_physics(size.x/2 * scale_x), to_physics(size.y/2 * scale_y)), 0);

		return shape;
	} else if (strncmp(str, "circle:", 7) == 0) {
		b2CircleShape* shape = new b2CircleShape();
		// TODO: Support scaling in y?
		shape->m_radius = to_physics(atof(str + 7) * scale_x);
		
		return shape;
	}

	return NULL;
}
