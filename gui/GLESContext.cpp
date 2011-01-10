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
#include "GLESProgram.hpp"
#include "Widget.hpp"
#include "common/math.hpp"
#include "common/Exception.hpp"

#include "SDL.h"
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>

using namespace LM;
using namespace std;

GLuint GLESContext::m_vbo = 0;

GLESContext::GLESContext(int width, int height, bool genfb) {
	m_width = width;
	m_height = height;

	m_stencil_depth = 0;
	m_stencil_type = GL_GEQUAL;

	m_bound_img = 0;
	m_img_bound = false;

	m_using_vbo = false;
	m_active_vbo = INVALID_VBO;

	m_color = Color::WHITE;

	if (genfb) {
		glGenFramebuffersEXT(1, &m_fbo);
		glGenRenderbuffersEXT(1, &m_stencil_rbo);
		glGenTextures(1, &m_fbo_tex);
	
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_fbo);

		glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, m_stencil_rbo);
		glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_STENCIL_INDEX8_EXT, width, height);

		glBindTexture(GL_TEXTURE_2D, m_fbo_tex);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, m_fbo_tex, 0);
	} else {
		m_fbo = 0;
		m_stencil_rbo = 0;
		m_fbo_tex = 0;

		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	}

	if (!m_vbo) {
		GLfloat vertices[] = {
			0, 0, 0, 0, // BUFFER
			-0.5f, -0.5f,
			0.5f, -0.5f,
			0.5f, 0.5f,
			-0.5f, 0.5f,
			0, 0,
			1, 0,
			1, 1,
			0, 1
		};
		glGenBuffers(1, &m_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		glTexCoordPointer(2, GL_FLOAT, 0, (GLvoid*)RECT_TEXS);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	glEnableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glViewport(0, 0, width, height);
	glEnable(GL_STENCIL_TEST);
	glEnable(GL_BLEND);
	set_blend_mode(BLEND_NORMAL);
}

GLESContext::~GLESContext() {
	if (m_fbo) {
		glDeleteFramebuffersEXT(1, &m_fbo);
		m_fbo = 0;
	}
	if (m_stencil_rbo) {
		glDeleteRenderbuffersEXT(1, &m_stencil_rbo);
		m_stencil_rbo = 0;
	}
	if (m_fbo_tex) {
		glDeleteTextures(1, &m_fbo_tex);
		m_fbo_tex = 0;
	}
}

void GLESContext::update_stencil() {
	glStencilFunc(m_stencil_type, m_stencil_depth + m_stencil_func, 0xFF);
}

void GLESContext::bind_vbo(VBOOffset offset) {
	if (!m_using_vbo) {
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		m_using_vbo = true;
	}
	if (offset != m_active_vbo) {
		glVertexPointer(2, GL_FLOAT, 0, (GLvoid*)offset);
		m_active_vbo = offset;
	}
}

void GLESContext::unbind_vbo() {
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	m_using_vbo = false;
	m_active_vbo = INVALID_VBO;
}

void GLESContext::reset_vbo() {
	if (m_active_vbo != INVALID_VBO) {
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		glVertexPointer(2, GL_FLOAT, 0, (GLvoid*)m_active_vbo);
	}
	if (!m_using_vbo) {
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
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
	unbind_vbo();
	glVertexPointer(2, GL_FLOAT, 0, m_arc_vertices);
}

void GLESContext::bind_rect(float w, float h) {
	bind_vbo(RECT_VERTS);

	glPushMatrix();
	glScalef(w, h, 1);
}

void GLESContext::unbind_rect() {
	glPopMatrix();
}

void GLESContext::draw_subimage(int width, int height, float tex_x, float tex_y, float tex_width, float tex_height) {
	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();

	glTranslatef(-tex_x/tex_width, -tex_y/tex_height, 0);
	glScalef(width/(tex_width), height/(tex_height), 1.0);

	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
}

unsigned char* GLESContext::setup_texture(PixelFormat fmt, const unsigned char* data,
										  int* w, int* h,
										  GLint* bpc, GLint* ifmt,
										  GLenum* glfmt, GLenum* type) {
	int nwidth = to_pow_2(*w);
	int nheight = to_pow_2(*h);
	// 2px textures seem to break things
	if (nwidth < 4) {
		nwidth = 4;
	}
	if (nheight < 4) {
		nheight = 4;
	}

	// XXX don't break const correctness
	unsigned char* ndata = const_cast<unsigned char*>(data);

	switch (fmt) {
	case RGBA:
		*bpc = 4;
		*ifmt = 4;
		*glfmt = GL_RGBA;
		*type = GL_UNSIGNED_BYTE;
		break;
	case ALPHA:
		*bpc = 1;
		*ifmt = GL_ALPHA8;
		*glfmt = GL_ALPHA;
		*type = GL_UNSIGNED_BYTE;
		break;
	default:
		throw new Exception("Invalid image format");
	}
	

	if (nwidth != *w || nheight != *h) {
		ndata = new unsigned char[nwidth*nheight* *bpc];
		for (int y = 0; y < *h; ++y) {
			memcpy(&ndata[nwidth * y * *bpc], &data[*w * y * *bpc], *w * *bpc);
			memset(&ndata[(nwidth * y + *w)* *bpc], 0, (nwidth - *w) * *bpc);
		}
		for (int y = *h; y < nheight; ++y) {
			memset(&ndata[nwidth * y * *bpc], 0, nwidth * *bpc);
		}
	}

	*w = nwidth;
	*h = nheight;
	return ndata;
}

void GLESContext::make_active() {
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_fbo);
	glViewport(0, 0, m_width, m_height);
	glBindTexture(GL_TEXTURE_2D, m_bound_img);
	glColor4f(m_color.r, m_color.g, m_color.b, m_color.a);
	set_blend_mode(m_mode);
	reset_vbo();
}

Image GLESContext::get_image(const string& name, ResourceCache* cache) {
	if (m_fbo_tex) {
		return LM::Image(m_width, m_height, name, cache, m_fbo_tex);
	} else {
		return LM::Image();
	}
}

int GLESContext::get_width() const {
	return m_width;
}

int GLESContext::get_height() const {
	return m_height;
}

void GLESContext::set_active_camera() {
	glMatrixMode(GL_PROJECTION);
}

void GLESContext::set_active_graphics() {
	glMatrixMode(GL_MODELVIEW);
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

void GLESContext::start_clip() {
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	++m_stencil_depth;
	clip_add();
}

void GLESContext::clip_add() {
	m_stencil_func = -1;
	glStencilOp(GL_KEEP, GL_INCR, GL_INCR);
	update_stencil();

	//set_draw_color(Color(1, 0, 0, 1));
}

void GLESContext::clip_sub() {
	m_stencil_func = 0;
	glStencilOp(GL_KEEP, GL_DECR, GL_DECR);
	update_stencil();

	//set_draw_color(Color(0, 0, 1, 1));
}

void GLESContext::finish_clip() {
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
	--m_stencil_depth;
	update_stencil();
}

void GLESContext::invert_clip() {
	if (m_stencil_type == GL_GEQUAL) {
		m_stencil_type = GL_LEQUAL;
	} else if (m_stencil_type == GL_LEQUAL) {
		m_stencil_type = GL_GEQUAL;
	}

	update_stencil();
}

void GLESContext::push_clip() {
	++m_stencil_depth;
	update_stencil();
}

void GLESContext::pop_clip() {
	--m_stencil_depth;
	update_stencil();
}

int GLESContext::clip_depth() {
	return m_stencil_depth;
}

void GLESContext::translate(float x, float y) {
	glTranslatef(x, y, 0);
}

void GLESContext::scale(float x, float y) {
	glScalef(x, y, 1);
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

void GLESContext::set_draw_color(Color c) {
	glColor4f(c.r, c.g, c.b, c.a);
	m_color = c;
}

void GLESContext::set_blend_mode(BlendMode m) {
	switch (m) {
		case BLEND_NORMAL:
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		break;

		case BLEND_ADD:
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		break;

		case BLEND_MULTIPLY:
		glBlendFunc(GL_DST_COLOR, GL_ZERO);
		break;

		case BLEND_SCREEN:
		glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_COLOR);
		break;
	}
	m_mode = m;
}

PixelShader GLESContext::load_pixel_shader(const std::string& filename) {
	GLuint shader = glCreateShader(GL_FRAGMENT_SHADER);
	string current;
	stringstream code;
	ifstream src(filename.c_str());
	while (!src.eof()) {
		getline(src, current);
		code << current << "\n";
	}
	code << endl;

	current = code.str();
	const char* codechars = current.c_str();
	cout << current << endl;
	glShaderSource(shader, 1, &codechars, NULL);
	glCompileShader(shader);

	GLint compiled;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
	if (!compiled) {
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &compiled);
		char* log = new char[compiled];
		glGetShaderInfoLog(shader, compiled, &compiled, log);
		cout << log << endl;
	}
	return shader;
}

void GLESContext::delete_pixel_shader(PixelShader shader) {
	glDeleteShader(shader);
}
	
ShaderSet* GLESContext::create_shader_set() {
	return new GLESProgram;
}

void GLESContext::bind_shader_set(ShaderSet* shaders) {
	glUseProgram(((GLESProgram*) shaders)->program_number());
}

void GLESContext::unbind_shader_set() {
	glUseProgram(0);
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
	bind_rect(w, h);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glDrawArrays(GL_LINE_LOOP, 0, 4);
	unbind_rect();
}

void GLESContext::draw_rect_fill(float w, float h) {
	bind_rect(w, h);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	unbind_rect();
}

void GLESContext::draw_rect_line(float w, float h) {
	bind_rect(w, h);
	glDrawArrays(GL_LINE_LOOP, 0, 4);
	unbind_rect();
}

void GLESContext::draw_line(float x1, float y1, float x2, float y2) {
	static GLfloat vertices[4];
	vertices[0] = x1;
	vertices[1] = y1;
	vertices[2] = x2;
	vertices[3] = y2;
	unbind_vbo();
	glVertexPointer(2, GL_FLOAT, 0, vertices);
	glDrawArrays(GL_LINE_STRIP, 0, 2);
}

void GLESContext::draw_lines(const float vertices[], int n, bool loop) {
	unbind_vbo();
	glVertexPointer(2, GL_FLOAT, 0, vertices);
	glDrawArrays(loop?GL_LINE_LOOP:GL_LINE_STRIP, 0, n);
}

void GLESContext::draw_polygon(const float vertices[], int n) {
	unbind_vbo();
	glVertexPointer(2, GL_FLOAT, 0, vertices);
	glDrawArrays(GL_POLYGON, 0, n);
	glDrawArrays(GL_LINE_LOOP, 0, n);
}

void GLESContext::draw_polygon_fill(const float vertices[], int n) {
	unbind_vbo();
	glVertexPointer(2, GL_FLOAT, 0, vertices);
	glDrawArrays(GL_POLYGON, 0, n);
}

DrawContext::Image GLESContext::gen_image(int* width, int* height, PixelFormat format, const unsigned char* data) {
	int w = *width;
	int h = *height;
	GLint	bpc;
	GLint	ifmt;
	GLenum	glfmt;
	GLenum	type;

	unsigned char* ndata = setup_texture(format, data, &w, &h, &bpc, &ifmt, &glfmt, &type);

	GLuint img;
	glGenTextures(1, &img);
	bind_image(img);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	
	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
	glTexImage2D(GL_TEXTURE_2D, 0, ifmt, w, h, 0, glfmt, type, ndata);
	if (ndata != data) {
		delete[] ndata;
	}
	unbind_image();
	*width = w;
	*height = h;
	return img;
}

void GLESContext::add_mipmap(Image handle, int level, int* width, int* height, PixelFormat format, const unsigned char* data) {
	int nwidth = *width;
	int nheight = *height;
	GLint bpc;
	GLint ifmt;
	GLenum glfmt;
	GLenum type;
	unsigned char* ndata;
	ndata = setup_texture(format, data, &nwidth, &nheight, &bpc, &ifmt, &glfmt, &type);

	bind_image(handle);
	glTexImage2D(GL_TEXTURE_2D, level, ifmt, nwidth, nheight, 0, glfmt, type, ndata);
	if (ndata != data) {
		delete[] ndata;
	}
	unbind_image();
	*width = nwidth;
	*height = nheight;
}

void GLESContext::del_image(Image img) {
	glDeleteTextures(1, &img);
}

void GLESContext::draw_image(int width, int height, Image img) {
	bind_image(img);
	draw_bound_image(width, height);
	unbind_image();
}

void GLESContext::bind_image(Image img) {
	if (m_bound_img != img || !m_img_bound) {
		glBindTexture(GL_TEXTURE_2D, img);
		m_bound_img = img;
		bind_vbo(IMG_VERTS);
	}
	if (!m_img_bound) {
		glEnable(GL_TEXTURE_2D);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		m_img_bound = true;
	}
}

void GLESContext::unbind_image() {
	if (m_img_bound) {
		glDisable(GL_TEXTURE_2D);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		m_img_bound = false;
	}
}

void GLESContext::draw_bound_image(int width, int height) {
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glPushMatrix();
	glScalef(width, height, 1);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glPopMatrix();
}

void GLESContext::draw_bound_image_region(int width, int height,
										  float tex_x, float tex_y,
										  float tex_width, float tex_height) {
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glPushMatrix();
	glScalef(width, height, 1);

	draw_subimage(width, height, tex_x, tex_y, tex_width, tex_height);

	glPopMatrix();
}

void GLESContext::draw_bound_image_tiled(int width, int height,
										 float tex_x, float tex_y,
										 float tex_width, float tex_height) {
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glPushMatrix();
	glScalef(width, height, 1);

	draw_subimage(width, height, tex_x, tex_y, tex_width, tex_height);

	glPopMatrix();
}

void GLESContext::clear() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void GLESContext::redraw() {
	clear();
	load_identity();

	get_root_widget()->draw(this);
}
