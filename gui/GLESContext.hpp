/*
 * gui/GLESContext.hpp
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

#ifndef LM_GUI_GLESCONTEXT_HPP
#define LM_GUI_GLESCONTEXT_HPP

#include "DrawContext.hpp"
#include "client/compat_gl.h"

namespace LM {
	class GLESContext : public DrawContext {
		using DrawContext::Image;

	public:
		static const int MAX_ARC_FINE = 64;

	private:
		static const GLint	m_rect_tex_vertices[8];

		GLint	m_width;
		GLint	m_height;
		int		m_depth;

		GLfloat	m_arc_vertices[2*(MAX_ARC_FINE + 2)];
		GLfloat	m_rect_vertices[8];

		// Current state
		GLuint	m_bound_img;
		bool	m_img_bound;
		Color	m_color;
		BlendMode m_mode;

		void	prepare_arc(float len, float xr, float yr, int fine);
		void	prepare_rect(float w, float h);

		void	draw_subimage(int width, int height,
							  float tex_x, float tex_y,
							  float tex_width, float tex_height);
		
	public:
		GLESContext(int width, int height);
		virtual ~GLESContext();

		virtual void	make_active();

		virtual int		get_width() const;
		virtual int		get_height() const;

		virtual void	load_identity();
		virtual void	push_transform();
		virtual void	pop_transform();
		virtual void	clip();
		virtual void	unclip();
		virtual void	finish_clip();
		virtual int		clip_depth();

		virtual void	translate(float x, float y);
		virtual void	scale(float x, float y);
		virtual void	rotate(float degrees);
		virtual void	skew_x(float amount);
		virtual void	skew_y(float amount);

		virtual void	set_draw_color(Color c);
		virtual void	set_blend_mode(BlendMode m);

		virtual void	draw_arc(float circumf, float xr, float yr, int fine);
		virtual void	draw_arc_fill(float circumf, float xr, float yr, int fine);
		virtual void	draw_arc_line(float circumf, float xr, float yr, int fine);

		virtual void	draw_rect(float w, float h);
		virtual void	draw_rect_fill(float w, float h);
		virtual void	draw_rect_line(float w, float h);

		virtual void	draw_line(float x1, float y1, float x2, float y2);
		virtual void	draw_lines(float vertices[], int n, bool loop);

		virtual Image	gen_image(int* width, int* height, PixelFormat format, unsigned char* data);
		virtual void	del_image(Image img);

		virtual void	bind_image(Image img);
		virtual void	unbind_image();
		virtual void	draw_image(int width, int height, Image img);
		virtual void	draw_bound_image(int width, int height);
		virtual void	draw_bound_image_region(int width, int height,
												float tex_x, float tex_y,
												float tex_width, float tex_height);
		virtual void	draw_bound_image_tiled(int width, int height,
											   float tex_x, float tex_y,
											   float tex_width, float tex_height);

		// TODO more GL properties (framebuffers?)

		virtual void	redraw();
	};
}

#endif
