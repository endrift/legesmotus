/*
 * common/Circle.hpp
 *
 * This file is part of Leges Motus, a networked, 2D shooter set in zero gravity.
 * 
 * Copyright 2009-2011 Andrew Ayer, Nathan Partlan, Jeffrey Pfau
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

#ifndef LM_COMMON_CIRCLE_HPP
#define LM_COMMON_CIRCLE_HPP

#include "common/Shape.hpp"
#include "common/Point.hpp"

namespace LM {
	class Circle : public Shape {
	private:
		double		get_angle_of_incidence (const Circle& other_circle) const;
		double		get_angle_of_incidence (Point point_on_circle) const;

	public:
		Point		center;
		double		radius;

		Circle (Point center, double radius);


		double		get_circumference() const;
		double		get_area() const;


		virtual double	boundary_intersects_circle(const Circle& circle, double* angle) const;
		virtual double	solid_intersects_circle(const Circle& circle, double* angle) const;
		virtual Point	intersects_line(Point start, Point end, double* angle) const;
		virtual double	dist_from_circle(const Circle& circle) const;

		virtual void	rotate (double angle);
		virtual void	scale (double factor);
		virtual void	translate (double x, double y);

		virtual bool	is_centered () const { return true; }
	};
}

#endif
