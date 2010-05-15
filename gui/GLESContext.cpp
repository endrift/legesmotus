/*
 * gui/GLESContext.cpp
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

#include "GLESContext.hpp"
#include "SDL.h"
#include "common/math.hpp"

using namespace LM;

GLESContext::GLESContext(int width, int height) {
	m_width = width;
	m_height = height;
}

GLESContext::~GLESContext() {
}

void GLESContext::prepare_arc(float len, float xr, float yr, int fine) {
	m_arc_vertices[0] = 0.0;
	m_arc_vertices[1] = 0.0;

	if (fine > MAX_ARC_FINE) {
		fine = MAX_ARC_FINE;
	}
	for (int i = 0; i <= fine; ++i) {
		m_arc_vertices[(i + 1)*2 + 0] = xr*cos(len*i*2.0*M_PI/fine);
		m_arc_vertices[(i + 1)*2 + 1] = yr*sin(len*i*2.0*M_PI/fine);
	}
	glVertexPointer(2, GL_FLOAT, 0, m_arc_vertices);
}

void GLESContext::prepare_rect(float w, float h) {
	m_rect_vertices[0] = -w/2.0f;
	m_rect_vertices[1] = -h/2.0f;
	m_rect_vertices[2] = w/2.0f;
	m_rect_vertices[3] = -h/2.0f;
	m_rect_vertices[4] = w/2.0f;
	m_rect_vertices[5] = h/2.0f;
	m_rect_vertices[6] = -w/2.0f;
	m_rect_vertices[7] = h/2.0f;
	glVertexPointer(2, GL_FLOAT, 0, m_rect_vertices);
}

int GLESContext::get_width() const {
	return m_width;
}

int GLESContext::get_height() const {
	return m_height;
}

void GLESContext::load_identity() {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, m_width, m_height, 0, -1, 0xFFFF);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void GLESContext::push_transform() {
	glPushMatrix();
}

void GLESContext::pop_transform() {
	glPopMatrix();
}

void GLESContext::translate(float x, float y) {
	glTranslatef(x, y, 0);
}

void GLESContext::scale(float x, float y) {
	glScalef(x, y, 0);
}

void GLESContext::rotate(float degrees) {
	glRotatef(degrees, 0, 0, 1);
}

void GLESContext::set_draw_color(Color c) {
	glColor4f(c.r, c.g, c.b, c.a);
}

void GLESContext::draw_arc(float len, float xr, float yr, int fine) {
	prepare_arc(len, xr, yr, fine);
	glDrawArrays(GL_TRIANGLE_FAN, 0, fine + 2);
	glDrawArrays(GL_LINE_STRIP, 1, fine + 1);
}

void GLESContext::draw_arc_fill(float len, float xr, float yr, int fine) {
	prepare_arc(len, xr, yr, fine);
	glDrawArrays(GL_TRIANGLE_FAN, 0, fine + 2);
}

void GLESContext::draw_arc_line(float len, float xr, float yr, int fine) {
	prepare_arc(len, xr, yr, fine);
	glDrawArrays(GL_LINE_STRIP, 1, fine + 1);
}

void GLESContext::draw_rect(float w, float h) {
	prepare_rect(w, h);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glDrawArrays(GL_LINE_LOOP, 0, 4);
}

void GLESContext::draw_rect_fill(float w, float h) {
	prepare_rect(w, h);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void GLESContext::draw_rect_line(float w, float h) {
	prepare_rect(w, h);
	glDrawArrays(GL_LINE_LOOP, 0, 4);
}

void GLESContext::draw_line(float x1, float y1, float x2, float y2) {
	static GLfloat vertices[4];
	vertices[0] = x1;
	vertices[1] = y1;
	vertices[2] = x2;
	vertices[3] = y2;
	glVertexPointer(2, GL_FLOAT, 0, vertices);
	glDrawArrays(GL_LINE_STRIP, 0, 2);
}

void GLESContext::draw_lines(float vertices[], int n, bool loop) {
	glVertexPointer(2, GL_FLOAT, 0, vertices);
	glDrawArrays(loop?GL_LINE_LOOP:GL_LINE_STRIP, 0, n);
}

void GLESContext::redraw() {
	glClear(GL_COLOR_BUFFER_BIT);
	load_identity();
	// TODO
}
