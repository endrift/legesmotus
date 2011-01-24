/*
 * gui/GLESContext.cpp
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

using namespace LM;
using namespace std;

GLuint GLESContext::m_vbo = 0;
GLESContext* GLESContext::m_current = NULL;

GLESContext::GLESContext(int width, int height, bool genfb) {
	m_width = width;
	m_height = height;

	m_stencil_depth = 0;
	m_stencil_type = LM_GL(GEQUAL);

	m_bound_img = 0;
	m_img_bound = false;

	m_using_vbo = false;
	m_active_vbo = INVALID_VBO;

	m_color = Color::WHITE;
	m_last = NULL;

	if (genfb) {
		LM_glEXT(GenFramebuffers, (1, &m_fbo));
		LM_glEXT(GenRenderbuffers, (1, &m_stencil_rbo));
		LM_gl(GenTextures, (1, &m_fbo_tex));
	
		LM_glEXT(BindFramebuffer, (LM_GL_EXT(FRAMEBUFFER), m_fbo));

		LM_glEXT(BindRenderbuffer, (LM_GL_EXT(RENDERBUFFER), m_stencil_rbo));
		LM_glEXT(RenderbufferStorage, (LM_GL_EXT(RENDERBUFFER), LM_GL_EXT(STENCIL_INDEX8), width, height));

		LM_gl(BindTexture, (LM_GL(TEXTURE_2D), m_fbo_tex));
		LM_gl(TexParameteri, (LM_GL(TEXTURE_2D), LM_GL(TEXTURE_MIN_FILTER), LM_GL(LINEAR)));
		LM_gl(TexParameteri, (LM_GL(TEXTURE_2D), LM_GL(TEXTURE_MAG_FILTER), LM_GL(LINEAR)));	
		LM_gl(TexImage2D, (LM_GL(TEXTURE_2D), 0, LM_GL(RGBA), width, height, 0, LM_GL(RGBA), LM_GL(UNSIGNED_BYTE), NULL));

		LM_glEXT(FramebufferTexture2D, (LM_GL_EXT(FRAMEBUFFER), LM_GL_EXT(COLOR_ATTACHMENT0), LM_GL(TEXTURE_2D), m_fbo_tex, 0));
	} else {
		m_fbo = 0;
		m_stencil_rbo = 0;
		m_fbo_tex = 0;

		LM_glEXT(BindFramebuffer, (LM_GL(FRAMEBUFFER), 0));
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
		LM_gl(GenBuffers, (1, &m_vbo));
		LM_gl(BindBuffer, (LM_GL(ARRAY_BUFFER), m_vbo));
		LM_gl(BufferData, (LM_GL(ARRAY_BUFFER), sizeof(vertices), vertices, LM_GL(STATIC_DRAW)));
		LM_gl(TexCoordPointer, (2, LM_GL(FLOAT), 0, (GLvoid*)RECT_TEXS));
		LM_gl(BindBuffer, (LM_GL(ARRAY_BUFFER), 0));
	}

	if (m_current == NULL) {
		m_current = this;
	}

	LM_gl(EnableClientState, (LM_GL(VERTEX_ARRAY)));
	LM_gl(DisableClientState, (LM_GL(TEXTURE_COORD_ARRAY)));
	LM_gl(Viewport, (0, 0, width, height));
	LM_gl(Scissor, (0, 0, width, height));
	//LM_gl(Enable, (LM_GL(SCISSOR_TEST)));
	LM_gl(Enable, (LM_GL(STENCIL_TEST)));
	LM_gl(Enable, (LM_GL(BLEND)));
	set_blend_mode(BLEND_NORMAL);

	if (m_current != this) {
		LM_glEXT(BindFramebuffer, (LM_GL_EXT(FRAMEBUFFER), m_current->m_fbo));
	}
}

GLESContext::~GLESContext() {
	if (m_fbo) {
		LM_glEXT(DeleteFramebuffers, (1, &m_fbo));
		m_fbo = 0;
	}
	if (m_stencil_rbo) {
		LM_glEXT(DeleteRenderbuffers, (1, &m_stencil_rbo));
		m_stencil_rbo = 0;
	}
	if (m_fbo_tex) {
		LM_gl(DeleteTextures, (1, &m_fbo_tex));
		m_fbo_tex = 0;
	}
}

void GLESContext::update_stencil() {
	LM_gl(StencilFunc, (m_stencil_type, m_stencil_depth + m_stencil_func, 0xFF));
}

void GLESContext::bind_vbo(VBOOffset offset) {
	if (!m_using_vbo) {
		LM_gl(BindBuffer, (LM_GL(ARRAY_BUFFER), m_vbo));
		m_using_vbo = true;
	}
	if (offset != m_active_vbo) {
		LM_gl(VertexPointer, (2, LM_GL(FLOAT), 0, (GLvoid*)offset));
		m_active_vbo = offset;
	}
}

void GLESContext::unbind_vbo() {
	LM_gl(BindBuffer, (LM_GL(ARRAY_BUFFER), 0));
	m_using_vbo = false;
	m_active_vbo = INVALID_VBO;
}

void GLESContext::reset_vbo() {
	if (m_active_vbo != INVALID_VBO) {
		LM_gl(BindBuffer, (LM_GL(ARRAY_BUFFER), m_vbo));
		LM_gl(VertexPointer, (2, LM_GL(FLOAT), 0, (GLvoid*)m_active_vbo));
	}
	if (!m_using_vbo) {
		LM_gl(BindBuffer, (LM_GL(ARRAY_BUFFER), 0));
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
	LM_gl(VertexPointer, (2, LM_GL(FLOAT), 0, m_arc_vertices));
}

void GLESContext::bind_rect(float w, float h) {
	bind_vbo(RECT_VERTS);

	push_transform();
	LM_gl(Scalef, (w, h, 1));
}

void GLESContext::unbind_rect() {
	pop_transform();
}

void GLESContext::draw_subimage(int width, int height, float tex_x, float tex_y, float tex_width, float tex_height) {
	LM_gl(MatrixMode, (LM_GL(TEXTURE)));
	LM_gl(LoadIdentity, ());

	LM_gl(Translatef, (-tex_x/tex_width, -tex_y/tex_height, 0));
	LM_gl(Scalef, (width/(tex_width), height/(tex_height), 1.0));

	LM_gl(DrawArrays, (LM_GL(TRIANGLE_FAN), 0, 4));

	LM_gl(LoadIdentity, ());
	LM_gl(MatrixMode, (LM_GL(MODELVIEW)));
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
		*ifmt = LM_GL(RGBA);
		*glfmt = LM_GL(RGBA);
		*type = LM_GL(UNSIGNED_BYTE);
		break;
	case ALPHA:
		*bpc = 1;
		*ifmt = LM_GL(ALPHA);
		*glfmt = LM_GL(ALPHA);
		*type = LM_GL(UNSIGNED_BYTE);
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
	LM_glEXT(BindFramebuffer, (LM_GL_EXT(FRAMEBUFFER), m_fbo));
	LM_gl(Viewport, (0, 0, m_width, m_height));
	LM_gl(Scissor, (0, 0, m_width, m_height));
	LM_gl(BindTexture, (LM_GL(TEXTURE_2D), m_bound_img));
	LM_gl(Color4f, (m_color.r, m_color.g, m_color.b, m_color.a));
	set_blend_mode(m_mode);
	reset_vbo();

	m_current = this;
}

void GLESContext::reset_persp() {
	LM_gl(LoadIdentity, ());
	// OpenGL ES doesn't have glOrtho
	/*GLfloat left = 0;
	GLfloat right = m_width;
	GLfloat bottom = m_height;
	GLfloat top = 0;
	GLfloat near = -0x7FFF;
	GLfloat far = 1;
	GLfloat mtx[] = {
		2.0f/(right-left), 0.0f, 0.0f, -(right+left)/(right-left),
		0.0f, 2.0f/(top-bottom), 0.0f, -(top+bottom)/(top-bottom),
		0.0f, 0.0f, -2.0f/(far-near), -(far+near)/(far-near),
		0.0f, 0.0f, 0.0f, 1.0f
	};
	LM_gl(MultMatrixf, (mtx));*/
	LM_gl(Ortho, (0, m_width, m_height, 0, -0x7FFF, 1));
}

void GLESContext::push_context() {
	LM_gl(MatrixMode, (LM_GL(PROJECTION)));
	push_transform();
	reset_persp();
	LM_gl(MatrixMode, (LM_GL(MODELVIEW)));
	push_transform();
	LM_gl(LoadIdentity, ());

	ASSERT(m_current != this);
	m_last = m_current;

	make_active();
}

void GLESContext::pop_context() {
	LM_gl(MatrixMode, (LM_GL(PROJECTION)));
	pop_transform();
	LM_gl(MatrixMode, (LM_GL(MODELVIEW)));
	pop_transform();

	m_last->make_active();
	m_current = m_last;
}

GLESContext* GLESContext::make_new_context(int width, int height) {
	return new GLESContext(width, height, true);
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
	LM_gl(MatrixMode, (LM_GL(PROJECTION)));
}

void GLESContext::set_active_graphics() {
	LM_gl(MatrixMode, (LM_GL(MODELVIEW)));
}

void GLESContext::load_identity() {
	LM_gl(MatrixMode, (LM_GL(PROJECTION)));
	reset_persp();

	LM_gl(MatrixMode, (LM_GL(MODELVIEW)));
	LM_gl(LoadIdentity, ());
}

void GLESContext::push_transform() {
	LM_gl(PushMatrix, ());
}

void GLESContext::pop_transform() {
	LM_gl(PopMatrix, ());
}

void GLESContext::start_clip() {
	LM_gl(ColorMask, (LM_GL(FALSE), LM_GL(FALSE), LM_GL(FALSE), LM_GL(FALSE)));
	++m_stencil_depth;
	clip_add();
}

void GLESContext::clip_add() {
	m_stencil_func = -1;
	LM_gl(StencilOp, (LM_GL(KEEP), LM_GL(INCR), LM_GL(INCR)));
	update_stencil();

	//set_draw_color(Color(1, 0, 0, 1));
}

void GLESContext::clip_sub() {
	m_stencil_func = 0;
	LM_gl(StencilOp, (LM_GL(KEEP), LM_GL(DECR), LM_GL(DECR)));
	update_stencil();

	//set_draw_color(Color(0, 0, 1, 1));
}

void GLESContext::finish_clip() {
	LM_gl(ColorMask, (LM_GL(TRUE), LM_GL(TRUE), LM_GL(TRUE), LM_GL(TRUE)));
	LM_gl(StencilOp, (LM_GL(KEEP), LM_GL(KEEP), LM_GL(KEEP)));
	--m_stencil_depth;
	update_stencil();
}

void GLESContext::invert_clip() {
	if (m_stencil_type == LM_GL(GEQUAL)) {
		m_stencil_type = LM_GL(LEQUAL);
	} else if (m_stencil_type == LM_GL(LEQUAL)) {
		m_stencil_type = LM_GL(GEQUAL);
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
	LM_gl(Translatef, (x, y, 0));
}

void GLESContext::scale(float x, float y) {
	LM_gl(Scalef, (x, y, 1));
}

void GLESContext::rotate(float degrees) {
	LM_gl(Rotatef, (degrees, 0, 0, 1));
}

void GLESContext::skew_x(float amount) {
	float mat[16] = {  1,      0, 0, 0,
					  -amount, 1, 0, 0,
					   0,      0, 1, 0,
					   0,      0, 0, 1 };
	LM_gl(MultMatrixf, (mat));
}

void GLESContext::skew_y(float amount) {
	float mat[16] = { 1, -amount, 0, 0,
					  0,       1, 0, 0,
					  0,       0, 1, 0,
					  0,       0, 0, 1 };
	LM_gl(MultMatrixf, (mat));
}

void GLESContext::set_draw_color(Color c) {
	LM_gl(Color4f, (c.r, c.g, c.b, c.a));
	m_color = c;
}

void GLESContext::set_blend_mode(BlendMode m) {
	switch (m) {
	case BLEND_NORMAL:
		LM_gl(BlendEquation, (LM_GL(FUNC_ADD)));
		LM_gl(BlendFunc, (LM_GL(SRC_ALPHA), LM_GL(ONE_MINUS_SRC_ALPHA)));
		break;

	case BLEND_ADD:
		LM_gl(BlendEquation, (LM_GL(FUNC_ADD)));
		LM_gl(BlendFunc, (LM_GL(SRC_ALPHA), LM_GL(ONE)));
		break;

	case BLEND_SUBTRACT:
		LM_gl(BlendEquation, (LM_GL(FUNC_REVERSE_SUBTRACT)));
		LM_gl(BlendFunc, (LM_GL(SRC_ALPHA), LM_GL(ONE)));
		break;

	case BLEND_SCREEN:
		LM_gl(BlendEquation, (LM_GL(FUNC_ADD)));
		LM_gl(BlendFunc, (LM_GL(SRC_ALPHA), LM_GL(ONE_MINUS_SRC_COLOR)));
		break;
	}
	m_mode = m;
}

const char* GLESContext::shader_directory() const {
	return "shaders/gl";
}

PixelShader GLESContext::load_pixel_shader(const std::string& filename) {
	GLuint shader = glCreateShader(LM_GL(FRAGMENT_SHADER));
	string current;
	stringstream code;
	ifstream src((filename + ".frag").c_str());
	while (!src.eof()) {
		if (src.fail()) {
			LM_gl(DeleteShader, (shader));
			WARN("Failed to load shader");
			return 0;
		}
		getline(src, current);
		code << current << "\n";
	}
	code << endl;

	current = code.str();
	const char* codechars = current.c_str();
	LM_gl(ShaderSource, (shader, 1, &codechars, NULL));
	LM_gl(CompileShader, (shader));

	GLint compiled;
	LM_gl(GetShaderiv, (shader, LM_GL(COMPILE_STATUS), &compiled));
	if (!compiled) {
		LM_gl(GetShaderiv, (shader, LM_GL(INFO_LOG_LENGTH), &compiled));
		char* log = new char[compiled];
		LM_gl(GetShaderInfoLog, (shader, compiled, &compiled, log));
		WARN("Shader compilation failed: " << log);
		delete[] log;
	}
	return shader;
}

void GLESContext::delete_pixel_shader(PixelShader shader) {
	LM_gl(DeleteShader, (shader));
}
	
ShaderSet* GLESContext::create_shader_set() {
	return new GLESProgram;
}

void GLESContext::bind_shader_set(ShaderSet* shaders) {
	LM_gl(UseProgram, (((GLESProgram*) shaders)->program_number()));
}

void GLESContext::unbind_shader_set() {
	LM_gl(UseProgram, (0));
}

void GLESContext::draw_arc(float len, float xr, float yr, int fine) {
	prepare_arc(len, xr, yr, fine);
	LM_gl(DrawArrays, (LM_GL(TRIANGLE_FAN), 0, fine + 2));
	LM_gl(DrawArrays, (LM_GL(LINE_STRIP), 1, fine + 1));
}

void GLESContext::draw_arc_fill(float len, float xr, float yr, int fine) {
	prepare_arc(len, xr, yr, fine);
	LM_gl(DrawArrays, (LM_GL(TRIANGLE_FAN), 0, fine + 2));
}

void GLESContext::draw_arc_line(float len, float xr, float yr, int fine) {
	prepare_arc(len, xr, yr, fine);
	LM_gl(DrawArrays, (LM_GL(LINE_STRIP), 1, fine + 1));
}

void GLESContext::draw_rect(float w, float h) {
	bind_rect(w, h);
	LM_gl(DrawArrays, (LM_GL(TRIANGLE_FAN), 0, 4));
	LM_gl(DrawArrays, (LM_GL(LINE_LOOP), 0, 4));
	unbind_rect();
}

void GLESContext::draw_rect_fill(float w, float h) {
	bind_rect(w, h);
	LM_gl(DrawArrays, (LM_GL(TRIANGLE_FAN), 0, 4));
	unbind_rect();
}

void GLESContext::draw_rect_line(float w, float h) {
	bind_rect(w, h);
	LM_gl(DrawArrays, (LM_GL(LINE_LOOP), 0, 4));
	unbind_rect();
}

void GLESContext::draw_line(float x1, float y1, float x2, float y2) {
	static GLfloat vertices[4];
	vertices[0] = x1;
	vertices[1] = y1;
	vertices[2] = x2;
	vertices[3] = y2;
	unbind_vbo();
	LM_gl(VertexPointer, (2, LM_GL(FLOAT), 0, vertices));
	LM_gl(DrawArrays, (LM_GL(LINE_STRIP), 0, 2));
}

void GLESContext::draw_lines(const float vertices[], int n, bool loop) {
	unbind_vbo();
	LM_gl(VertexPointer, (2, LM_GL(FLOAT), 0, vertices));
	LM_gl(DrawArrays, (loop?LM_GL(LINE_LOOP):LM_GL(LINE_STRIP), 0, n));
}

void GLESContext::draw_stroke(const float vertices[], int n, float out, float in, bool loop) {
	ASSERT(sizeof(GLfloat) == sizeof(float));
	float quad_vertices[(n+1)*4];

	for (int i = 0; i < n; ++i) {
		int j = (i + 1) % n;
		int k = (i + 2) % n;
		Point t1(vertices[2*i], vertices[2*i + 1]);
		Point t2(vertices[2*j], vertices[2*j + 1]);
		Point t3(vertices[2*k], vertices[2*k + 1]);
		Point i1, i21, i22, i3;
		Point o1, o21, o22, o3;
		Vector p12 = t2 - t1;
		Vector p23 = t3 - t2;
		Point u;
		u = p12;
		p12.x = -u.y;
		p12.y = u.x;
		p12 /= u.get_magnitude();
		u = p23;
		p23.x = -u.y;
		p23.y = u.x;
		p23 /= u.get_magnitude();

		i1 = t1 + p12*in;
		i21 = t2 + p12*in;
		i22 = t2 + p23*in;
		i3 = t3 + p23*in;

		o1 = t1 - p12*out;
		o21 = t2 - p12*out;
		o22 = t2 - p23*out;
		o3 = t3 - p23*out;

		intersection(i1, i21, i22, i3, &u);
		quad_vertices[4*j] = u.x;
		quad_vertices[4*j + 1] = u.y;

		intersection(o1, o21, o22, o3, &u);
		quad_vertices[4*j + 2] = u.x;
		quad_vertices[4*j + 3] = u.y;
	}

	if (loop) {
		++n;
		
		quad_vertices[4*n-4] = quad_vertices[0];
		quad_vertices[4*n-3] = quad_vertices[1];
		quad_vertices[4*n-2] = quad_vertices[2];
		quad_vertices[4*n-1] = quad_vertices[3];
	}

	unbind_vbo();
	LM_gl(VertexPointer, (2, LM_GL(FLOAT), 0, quad_vertices));
	LM_gl(DrawArrays, (LM_GL(QUAD_STRIP), 0, n*2));
}

void GLESContext::draw_polygon(const float vertices[], int n) {
	unbind_vbo();
	LM_gl(VertexPointer, (2, LM_GL(FLOAT), 0, vertices));
	LM_gl(DrawArrays, (LM_GL(POLYGON), 0, n));
	LM_gl(DrawArrays, (LM_GL(LINE_LOOP), 0, n));
}

void GLESContext::draw_polygon_fill(const float vertices[], int n) {
	unbind_vbo();
	LM_gl(VertexPointer, (2, LM_GL(FLOAT), 0, vertices));
	LM_gl(DrawArrays, (LM_GL(POLYGON), 0, n));
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
	LM_gl(GenTextures, (1, &img));
	bind_image(img);
	LM_gl(TexParameteri, (LM_GL(TEXTURE_2D), LM_GL(TEXTURE_MIN_FILTER), LM_GL(LINEAR_MIPMAP_LINEAR)));
	LM_gl(TexParameteri, (LM_GL(TEXTURE_2D), LM_GL(TEXTURE_MAG_FILTER), LM_GL(LINEAR)));	
	LM_gl(TexParameteri, (LM_GL(TEXTURE_2D), LM_GL(GENERATE_MIPMAP), LM_GL(TRUE)));
	LM_gl(TexImage2D, (LM_GL(TEXTURE_2D), 0, ifmt, w, h, 0, glfmt, type, ndata));
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
	LM_gl(TexImage2D, (LM_GL(TEXTURE_2D), level, ifmt, nwidth, nheight, 0, glfmt, type, ndata));
	if (ndata != data) {
		delete[] ndata;
	}
	unbind_image();
	*width = nwidth;
	*height = nheight;
}

void GLESContext::del_image(Image img) {
	LM_gl(DeleteTextures, (1, &img));
}

void GLESContext::draw_image(int width, int height, Image img) {
	bind_image(img);
	draw_bound_image(width, height);
	unbind_image();
}

void GLESContext::bind_image(Image img) {
	if (m_bound_img != img || !m_img_bound) {
		LM_gl(BindTexture, (LM_GL(TEXTURE_2D), img));
		m_bound_img = img;
		bind_vbo(IMG_VERTS);
	}
	if (!m_img_bound) {
		LM_gl(Enable, (LM_GL(TEXTURE_2D)));
		LM_gl(EnableClientState, (LM_GL(TEXTURE_COORD_ARRAY)));
		m_img_bound = true;
	}
}

void GLESContext::unbind_image() {
	if (m_img_bound) {
		LM_gl(Disable, (LM_GL(TEXTURE_2D)));
		LM_gl(DisableClientState, (LM_GL(TEXTURE_COORD_ARRAY)));
		m_img_bound = false;
	}
}

void GLESContext::draw_bound_image(int width, int height) {
	LM_gl(TexParameteri, (LM_GL(TEXTURE_2D), LM_GL(TEXTURE_WRAP_S), LM_GL(CLAMP_TO_EDGE)));
	LM_gl(TexParameteri, (LM_GL(TEXTURE_2D), LM_GL(TEXTURE_WRAP_T), LM_GL(CLAMP_TO_EDGE)));
	push_transform();
	LM_gl(Scalef, (width, height, 1));
	LM_gl(DrawArrays, (LM_GL(TRIANGLE_FAN), 0, 4));
	pop_transform();
}

void GLESContext::draw_bound_image_region(int width, int height,
										  float tex_x, float tex_y,
										  float tex_width, float tex_height) {
	LM_gl(TexParameteri, (LM_GL(TEXTURE_2D), LM_GL(TEXTURE_WRAP_S), LM_GL(CLAMP_TO_EDGE)));
	LM_gl(TexParameteri, (LM_GL(TEXTURE_2D), LM_GL(TEXTURE_WRAP_T), LM_GL(CLAMP_TO_EDGE)));
	push_transform();
	LM_gl(Scalef, (width, height, 1));

	draw_subimage(width, height, tex_x, tex_y, tex_width, tex_height);

	pop_transform();
}

void GLESContext::draw_bound_image_tiled(int width, int height,
										 float tex_x, float tex_y,
										 float tex_width, float tex_height) {
	LM_gl(TexParameteri, (LM_GL(TEXTURE_2D), LM_GL(TEXTURE_WRAP_S), LM_GL(REPEAT)));
	LM_gl(TexParameteri, (LM_GL(TEXTURE_2D), LM_GL(TEXTURE_WRAP_T), LM_GL(REPEAT)));
	push_transform();
	LM_gl(Scalef, (width, height, 1));

	draw_subimage(width, height, tex_x, tex_y, tex_width, tex_height);

	pop_transform();
}

void GLESContext::clear() {
	LM_gl(Clear, (LM_GL(COLOR_BUFFER_BIT) | LM_GL(DEPTH_BUFFER_BIT) | LM_GL(STENCIL_BUFFER_BIT)));
}

void GLESContext::redraw() {
	clear();
	load_identity();

	get_root_widget()->draw(this);
}
