/*
 * gui/primitives.cpp
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

#include "primitives.hpp"
#include "compat_gl.h"
#include "common/math.hpp"

using namespace LM;

static GLfloat arc_vertices[2*(MAX_ARC_FINE + 2)];
static GLfloat rect_vertices[8];

static void prepare_arc(float len, float xr, float yr, int fine) {
	arc_vertices[0] = 0.0;
	arc_vertices[1] = 0.0;

	if (fine > MAX_ARC_FINE) {
		fine = MAX_ARC_FINE;
	}
	for (int i = 0; i <= fine; ++i) {
		arc_vertices[(i + 1)*2 + 0] = xr*cos(len*i*2.0*M_PI/fine);
		arc_vertices[(i + 1)*2 + 1] = yr*sin(len*i*2.0*M_PI/fine);
	}
	glVertexPointer(2, GL_FLOAT, 0, arc_vertices);
}

static void prepare_rect(float w, float h) {
	rect_vertices[0] = -w/2.0f;
	rect_vertices[1] = -h/2.0f;
	rect_vertices[2] = w/2.0f;
	rect_vertices[3] = -h/2.0f;
	rect_vertices[4] = w/2.0f;
	rect_vertices[5] = h/2.0f;
	rect_vertices[6] = -w/2.0f;
	rect_vertices[7] = h/2.0f;
	glVertexPointer(2, GL_FLOAT, 0, rect_vertices);
}

void LM::draw_arc(float len, float xr, float yr, int fine) {
	prepare_arc(len, xr, yr, fine);
	glDrawArrays(GL_TRIANGLE_FAN, 0, fine + 2);
	glDrawArrays(GL_LINE_STRIP, 1, fine + 1);
}

void LM::draw_arc_fill(float len, float xr, float yr, int fine) {
	prepare_arc(len, xr, yr, fine);
	glDrawArrays(GL_TRIANGLE_FAN, 0, fine + 2);
}

void LM::draw_arc_line(float len, float xr, float yr, int fine) {
	prepare_arc(len, xr, yr, fine);
	glDrawArrays(GL_LINE_STRIP, 1, fine + 1);
}

void LM::draw_rect(float w, float h) {
	prepare_rect(w, h);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glDrawArrays(GL_LINE_LOOP, 0, 4);
}

void LM::draw_rect_fill(float w, float h) {
	prepare_rect(w, h);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void LM::draw_rect_line(float w, float h) {
	prepare_rect(w, h);
	glDrawArrays(GL_LINE_LOOP, 0, 4);
}

void LM::draw_line(float x1, float y1, float x2, float y2) {
	static GLfloat vertices[4];
	vertices[0] = x1;
	vertices[1] = y1;
	vertices[2] = x2;
	vertices[3] = y2;
	glVertexPointer(2, GL_FLOAT, 0, vertices);
	glDrawArrays(GL_LINE_STRIP, 0, 2);
}

void LM::draw_roundrect(float w, float h, float r, int fine) {
	draw_roundrect_fill(w, h, r, fine);
}

void LM::draw_roundrect_fill(float w, float h, float r, int fine) {
	if (w - 2.0f*r < 0) {
		r = w/2.0f;
	}
	if (h - 2.0f*r < 0) {
		r = h/2.0f;
	}

	float r_t2 = 2.0f*r;
	float r_o2 = 0.5f*r;

	// Draw center
	draw_rect_fill(w - r_t2, h - r_t2);

	// Draw sides
	glTranslatef(w/2.0f - r_o2, 0, 0);
	draw_rect_fill(r, h - r_t2);
	glTranslatef(r - w, 0, 0);
	draw_rect_fill(r, h - r_t2);
	glTranslatef(w/2.0f - r_o2, h/2.0f - r_o2, 0);
	draw_rect_fill(w - r_t2, r);
	glTranslatef(0, r - h, 0);
	draw_rect_fill(w - r_t2, r);

	// Recenter
	glTranslatef(0, h/2.0f - r_o2, 0);

	// Draw corners
	glTranslatef(w/2.0f - r, h/2.0f - r, 0);
	draw_arc_fill(0.25f, r, r, fine);

	glTranslatef(r_t2 - w, 0, 0);
	glRotatef(90, 0, 0, 1.0f);
	draw_arc_fill(0.25f, r, r, fine);

	glTranslatef(r_t2 - h, 0, 0);
	glRotatef(90, 0, 0, 1.0f);
	draw_arc_fill(0.25f, r, r, fine);

	glTranslatef(r_t2 - w, 0, 0);
	glRotatef(90, 0, 0, 1.0f);
	draw_arc_fill(0.25f, r, r, fine);

	// Recenter
	glTranslatef(r - h/2.0f, r - w/2.0f, 0);
	glRotatef(90, 0, 0, 1.0f);
}

void LM::draw_roundrect_line(float w, float h, float r, int fine) {
	if (w - 2.0f*r < 0) {
		r = w/2.0f;
	}
	if (h - 2.0f*r < 0) {
		r = h/2.0f;
	}

	float r_t2 = 2.0f*r;

	// Draw edges
	draw_line(w/2.0f, h/2.0f - r, w/2.0f, r - h/2.0f);
	draw_line(-w/2.0f, h/2.0f - r, -w/2.0f, r - h/2.0f);
	draw_line(w/2.0f - r, h/2.0f, r - w/2.0f, h/2.0f);
	draw_line(w/2.0f - r, -h/2.0f, r - w/2.0f, -h/2.0f);

	// Draw corners
	glTranslatef(w/2.0f - r, h/2.0f - r, 0);
	draw_arc_line(0.25f, r, r, fine);

	glTranslatef(r_t2 - w, 0, 0);
	glRotatef(90, 0, 0, 1.0f);
	draw_arc_line(0.25f, r, r, fine);

	glTranslatef(r_t2 - h, 0, 0);
	glRotatef(90, 0, 0, 1.0f);
	draw_arc_line(0.25f, r, r, fine);

	glTranslatef(r_t2 - w, 0, 0);
	glRotatef(90, 0, 0, 1.0f);
	draw_arc_line(0.25f, r, r, fine);

	// Recenter
	glTranslatef(r - h/2.0f, r - w/2.0f, 0);
	glRotatef(90, 0, 0, 1.0f);
}
