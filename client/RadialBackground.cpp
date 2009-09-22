/*
 * client/RadialBackground.cpp
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

#include "RadialBackground.hpp"
#include "common/Exception.hpp"
#include "common/math.hpp"

using namespace LM;
using namespace std;

const int RadialBackground::RESOLUTION = 64;

RadialBackground::RadialBackground(int num_segments) {
	m_num_segments = 0;
	set_num_segments(num_segments);
	m_border_radius = 1.0;
	m_border_angle = 0.0;
	m_outer_radius = 2.0;
	m_inner_radius = 1.0;
}

RadialBackground* RadialBackground::clone() const {
	return new RadialBackground(*this);
}

int RadialBackground::get_num_segments() const {
	return m_num_segments;
}

void RadialBackground::set_num_segments(int num_segments) {
	if (num_segments < 1) {
		return;
	}
	int old_num_segments = m_num_segments;
	m_segment_colors.resize(num_segments);
	m_num_segments = num_segments;
	for (int i = old_num_segments; i < num_segments; ++i) {
		m_segment_colors[i] = Color(0,0,0,0);
	}
}

void RadialBackground::set_border_radius(double radius) {
	m_border_radius = radius;
}

void RadialBackground::set_border_angle(double degrees) {
	m_border_angle = degrees*DEGREES_TO_RADIANS;
}

void RadialBackground::set_border_color(Color color) {
	m_border_color = color;
}

void RadialBackground::set_segment_color(int segment, Color color) {
	if (segment >= m_num_segments || segment < 0) {
		throw Exception("Segment out of bounds");
	}
	
	m_segment_colors[segment] = color;
}

void RadialBackground::set_outer_radius(double radius) {
	m_outer_radius = radius;
}

void RadialBackground::set_inner_radius(double radius) {
	m_inner_radius = radius;
}

double RadialBackground::get_outer_radius() const {
	return m_outer_radius;
}

double RadialBackground::get_inner_radius() const {
	return m_inner_radius;
}

void RadialBackground::draw(const GameWindow* window) const {
	(void)(window); // Unused
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST); // Enable depth test so parts aren't redrawn
	glClearDepth(-1);
	glDepthFunc(GL_GREATER);
	glClear(GL_DEPTH_BUFFER_BIT);

	glPushMatrix();
	transform_gl();
	int segment_res = RESOLUTION/m_num_segments;
	double segment_inner_radius = m_inner_radius + m_border_radius;
	double segment_outer_radius = m_inner_radius + m_outer_radius - m_border_radius;
	double segment_size = 2*M_PI/m_num_segments;
	double segment_scale = (2*M_PI-m_border_angle*m_num_segments)/(segment_res*m_num_segments);
	double segment_offset = m_border_angle*0.5;
	for (int i = 0; i < m_num_segments; ++i) {
		glColor4d(m_segment_colors[i].r, m_segment_colors[i].g, m_segment_colors[i].b, m_segment_colors[i].a);
		for (int j = 0; j < segment_res; ++j) {
			GLdouble vertices[2*4] = {
				cos(j*segment_scale+segment_offset+segment_size*i)*segment_inner_radius,
				sin(j*segment_scale+segment_offset+segment_size*i)*segment_inner_radius,
				cos(j*segment_scale+segment_offset+segment_size*i)*segment_outer_radius,
				sin(j*segment_scale+segment_offset+segment_size*i)*segment_outer_radius,
				cos((j + 1)*segment_scale+segment_offset+segment_size*i)*segment_inner_radius,
				sin((j + 1)*segment_scale+segment_offset+segment_size*i)*segment_inner_radius,
				cos((j + 1)*segment_scale+segment_offset+segment_size*i)*segment_outer_radius,
				sin((j + 1)*segment_scale+segment_offset+segment_size*i)*segment_outer_radius
			};
			glVertexPointer(2, GL_DOUBLE, 0, vertices);
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		}
	}

	glColor4d(m_border_color.r, m_border_color.g, m_border_color.b, m_border_color.a);
	for (int i = 0; i < RESOLUTION; ++i) {
		GLdouble vertices[2*4] = {
			cos(i*2*M_PI/(RESOLUTION))*m_inner_radius,
			sin(i*2*M_PI/(RESOLUTION))*m_inner_radius,
			cos(i*2*M_PI/(RESOLUTION))*(m_inner_radius + m_outer_radius),
			sin(i*2*M_PI/(RESOLUTION))*(m_inner_radius + m_outer_radius),
			cos((i + 1)*2*M_PI/(RESOLUTION))*m_inner_radius,
			sin((i + 1)*2*M_PI/(RESOLUTION))*m_inner_radius,
			cos((i + 1)*2*M_PI/(RESOLUTION))*(m_inner_radius + m_outer_radius),
			sin((i + 1)*2*M_PI/(RESOLUTION))*(m_inner_radius + m_outer_radius)
		};
		glVertexPointer(2, GL_DOUBLE, 0, vertices);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}

	glPopMatrix();

	glDisable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
}
