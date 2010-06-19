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
#include "common/Exception.hpp"

using namespace LM;

GLint GLESContext::m_rect_tex_vertices[] = {
	0, 0,
	1, 0,
	1, 1,
	0, 1
};

GLESContext::GLESContext(int width, int height) {
	m_width = width;
	m_height = height;
	m_depth = 0;

	m_bound_img = 0;

	//glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnable(GL_STENCIL_TEST);
	glBlendColor(1.0f, 1.0f, 1.0f, 1.0f);
	set_draw_mode(NORMAL);
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
	glTexCoordPointer(2, GL_FLOAT, 0, m_rect_tex_vertices);
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
	glOrtho(0, m_width, m_height, 0, -0x7FFF, 1);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void GLESContext::push_transform() {
	glPushMatrix();
}

void GLESContext::pop_transform() {
	glPopMatrix();
}

void GLESContext::clip() {
	glStencilOp(GL_KEEP, GL_INCR, GL_INCR);
	glStencilFunc(GL_EQUAL, m_depth, 0x7F);
	++m_depth;
}

void GLESContext::unclip() {
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glStencilOp(GL_KEEP, GL_DECR, GL_DECR);
	glStencilFunc(GL_EQUAL, m_depth, 0x7F);
	--m_depth;
}

void GLESContext::finish_clip() {
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
}

int GLESContext::clip_depth() {
	return m_depth;
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

void GLESContext::skew_x(float amount) {
	float mat[16] = {  1,      0, 0, 0,
					  -amount, 1, 0, 0,
					   0,      0, 1, 0,
					   0,      0, 0, 1 };
	glMultMatrixf(mat);
}

void GLESContext::skew_y(float amount) {
	float mat[16] = { 1, -amount, 0, 0,
					  0,       1, 0, 0,
					  0,       0, 1, 0,
					  0,       0, 0, 1 };
	glMultMatrixf(mat);
}

void GLESContext::set_screen_color(Color c) {
	glBlendColor(c.r, c.g, c.b, c.a);
}

void GLESContext::set_draw_color(Color c) {
	glColor4f(c.r, c.g, c.b, c.a);
}

void GLESContext::set_draw_mode(DrawMode m) {
	switch (m) {
		case NORMAL:
		glBlendFunc(GL_CONSTANT_COLOR, GL_ZERO);
		break;

		case MULTIPLY:
		glBlendFunc(GL_DST_COLOR, GL_ZERO);
		break;

		case ADD:
		glBlendFunc(GL_CONSTANT_COLOR, GL_ONE);
		break;
	}
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

DrawContext::Image GLESContext::gen_image(int* width, int* height, PixelFormat format, unsigned char* data) {
	int w = *width;
	int h = *height;
	int nwidth = to_pow_2(w);
	int nheight = to_pow_2(h);
	// 2px textures seem to break things
	if (nwidth < 4) {
		nwidth = 4;
	}
	if (nheight < 4) {
		nheight = 4;
	}
	GLint	bpc;
	GLint	ifmt;
	GLenum	glfmt;
	GLenum	type;
	unsigned char* ndata = data;

	switch (format) {
	case RGBA:
		bpc = 4;
		ifmt = 4;
		glfmt = GL_BGRA;
		type = GL_UNSIGNED_INT_8_8_8_8_REV;
		break;
	case ALPHA:
		bpc = 1;
		ifmt = GL_ALPHA8;
		glfmt = GL_ALPHA;
		type = GL_UNSIGNED_BYTE;
		break;
	default:
		throw new Exception("Invalid image format");
	}

	if (nwidth != w || nheight != h) {
		ndata = new unsigned char[nwidth*nheight*bpc];
		for (int y = 0; y < h; ++y) {
			memcpy(&ndata[nwidth*y*bpc], &data[w*y*bpc], w*bpc);
			memset(&ndata[(nwidth*y + w)*bpc], 0, (nwidth - w)*bpc);
		}
		for (int y = h; y < nheight; ++y) {
			memset(&ndata[nwidth*y*bpc], 0, nwidth*bpc);
		}
	}
	GLuint img;
	glGenTextures(1, &img);
	bind_image(img);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	
	glTexImage2D(GL_TEXTURE_2D, 0, ifmt, nwidth, nheight, 0, glfmt, type, ndata);
	if (ndata != data) {
		delete[] ndata;
	}
	*width = nwidth;
	*height = nheight;
	return img;
}

void GLESContext::del_image(Image img) {
	glDeleteTextures(1, &img);
}

void GLESContext::draw_image(int width, int height, Image img) {
	bind_image(img);
	draw_bound_image(width, height);
}

void GLESContext::bind_image(Image img) {
	//if (m_bound_img != img) {
		glBindTexture(GL_TEXTURE_2D, img);
		m_bound_img = img;
	//}
}

void GLESContext::draw_bound_image(int width, int height) {
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	GLint vertices[8] = {
		0, 0,
		width, 0,
		width, height,
		0, height
	};

	glVertexPointer(2, GL_INT, 0, vertices);
	glTexCoordPointer(2, GL_INT, 0, m_rect_tex_vertices);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void GLESContext::draw_bound_image_region(int width, int height,
										  float corner0x, float corner0y,
										  float corner1x, float corner1y) {
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	GLint vertices[8] = {
		0, 0,
		width, 0,
		width, height,
		0, height
	};
	GLfloat texcoords[8] = {
		corner0x, corner0y,
		corner1x, corner0y,
		corner1x, corner1y,
		corner0x, corner1y
	};

	glVertexPointer(2, GL_INT, 0, vertices);
	glTexCoordPointer(2, GL_FLOAT, 0, texcoords);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void GLESContext::redraw() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	load_identity();
	// TODO
}
