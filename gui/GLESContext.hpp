/*
 * gui/GLESContext.hpp
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

#ifndef LM_GUI_GLESCONTEXT_HPP
#define LM_GUI_GLESCONTEXT_HPP

#include "DrawContext.hpp"
#include "Image.hpp"

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glext.h>
//#include <OpenGLES/ES2/gl.h>
//#include <OpenGLES/ES2/glext.h>
#else
#define GL_GLEXT_PROTOTYPES
#include "GL/gl.h"
#include "GL/glu.h"
#include "GL/glext.h"
#ifdef __WIN32
#define GL_SRC0_RGB GL_SOURCE0_RGB
#define GL_SRC1_RGB GL_SOURCE1_RGB
#define GL_SRC2_RGB GL_SOURCE2_RGB
#define GL_SRC0_ALPHA GL_SOURCE0_ALPHA
#define GL_SRC1_ALPHA GL_SOURCE1_ALPHA
#define GL_SRC2_ALPHA GL_SOURCE2_ALPHA
#endif
#endif

#ifdef LM_DEBUG
#define LM_gl(call, args) \
	do {                  \
		GLenum e;         \
		gl ## call args;  \
		e = glGetError(); \
		if (e) {          \
			WARN("OpenGL error: 0x" << hex << e); \
		}                 \
	} while(0)
#else
#define LM_gl(call, args) gl ## call args
#endif

#define LM_glEXT(call, args) LM_gl(call ## EXT, args)

#define LM_GL(constant) GL_ ## constant
#define LM_GL_EXT(constant) GL_ ## constant ## _EXT

namespace LM {
	class GLESContext : public DrawContext {
		using DrawContext::Image;

	public:
		static const int MAX_ARC_FINE = 64;

	private:
		static GLuint m_vbo;
		static GLESContext* m_current;

		enum VBOOffset {
			INVALID_VBO = 0,
			RECT_VERTS = sizeof(GLfloat[4]),
			RECT_TEXS = sizeof(GLfloat[12]),
			IMG_VERTS = RECT_TEXS
		};

		GLint m_width;
		GLint m_height;

		GLfloat	m_arc_vertices[2*(MAX_ARC_FINE + 2)];

		// Buffer objects
		GLuint m_fbo;
		GLuint m_stencil_rbo;
		GLuint m_fbo_tex;

		GLESContext* m_last;

		VBOOffset m_active_vbo;
		bool m_using_vbo;

		// Current state
		GLuint	m_bound_img;
		bool	m_img_bound;
		Color	m_color;
		BlendMode m_mode;

		int		m_stencil_depth;
		int		m_stencil_func;
		GLenum	m_stencil_type;

		void	update_stencil();

		void	bind_vbo(VBOOffset offset);
		void	unbind_vbo();
		void	reset_vbo();

		void prepare_arc(float len, float xr, float yr, int fine);
		void bind_rect(float w, float h);
		void unbind_rect();

		void draw_subimage(int width, int height,
		                   float tex_x, float tex_y,
		                   float tex_width, float tex_height);

		unsigned char* setup_texture(PixelFormat fmt, const unsigned char* data,
		                             int* w, int* h, GLint* bpc, GLint* ifmt,
		                             GLenum* glfmt, GLenum* type);

		void make_active();

		void reset_persp();

	public:
		GLESContext(int width, int height, bool genfb = false);
		virtual ~GLESContext();

		virtual void push_context();
		virtual void pop_context();

		virtual LM::Image get_image(const std::string& name, ResourceCache* cache);
		virtual GLESContext* make_new_context(int width, int height);

		virtual int get_width() const;
		virtual int get_height() const;

		virtual void set_active_camera();
		virtual void set_active_graphics();

		virtual void load_identity();
		virtual void push_transform();
		virtual void pop_transform();
                     
		virtual void start_clip();
		virtual void clip_add();
		virtual void clip_sub();
		virtual void finish_clip();
		virtual void invert_clip();
		virtual void push_clip();
		virtual void pop_clip();
		virtual int clip_depth();

		virtual void translate(float x, float y);
		virtual void scale(float x, float y);
		virtual void rotate(float degrees);
		virtual void skew_x(float amount);
		virtual void skew_y(float amount);

		virtual void set_draw_color(Color c);
		virtual void set_blend_mode(BlendMode m);

		virtual const char* shader_directory() const;
		virtual PixelShader load_pixel_shader(const std::string& filename);
		virtual void delete_pixel_shader(PixelShader shader);

		virtual ShaderSet* create_shader_set();
		virtual void bind_shader_set(ShaderSet* shaders);
		virtual void unbind_shader_set();

		virtual void draw_arc(float circumf, float xr, float yr, int fine);
		virtual void draw_arc_fill(float circumf, float xr, float yr, int fine);
		virtual void draw_arc_line(float circumf, float xr, float yr, int fine);

		virtual void draw_rect(float w, float h);
		virtual void draw_rect_fill(float w, float h);
		virtual void draw_rect_line(float w, float h);

		virtual void draw_line(float x1, float y1, float x2, float y2);
		virtual void draw_lines(const float vertices[], int n, bool loop);
		virtual void draw_stroke(const float vertices[], int n, float out, float in, bool loop);
		virtual void draw_polygon(const float vertices[], int n);
		virtual void draw_polygon_fill(const float vertices[], int n);

		virtual Image gen_image(int* width, int* height, PixelFormat format, const unsigned char* data);
		virtual void add_mipmap(Image handle, int level, int* width, int* height, PixelFormat format, const unsigned char* data);
		virtual void del_image(Image img);

		virtual void bind_image(Image img);
		virtual void unbind_image();
		virtual void draw_image(int width, int height, Image img);
		virtual void draw_bound_image(int width, int height);
		virtual void draw_bound_image_region(int width, int height,
												float tex_x, float tex_y,
												float tex_width, float tex_height);
		virtual void draw_bound_image_tiled(int width, int height,
											   float tex_x, float tex_y,
											   float tex_width, float tex_height);

		virtual void clear();
		virtual void redraw();
	};
}

#endif
