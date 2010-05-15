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
#include "compat_gl.h"

namespace LM {
	class GLESContext : public DrawContext {
	public:
		static const int MAX_ARC_FINE = 64;

	private:
		GLint	m_width;
		GLint	m_height;

		GLfloat	m_arc_vertices[2*(MAX_ARC_FINE + 2)];
		GLfloat	m_rect_vertices[8];

		void	prepare_arc(float len, float xr, float yr, int fine);
		void	prepare_rect(float w, float h);
		
	public:
		GLESContext(int width, int height);
		virtual ~GLESContext();

		virtual int		get_width() const;
		virtual int		get_height() const;

		virtual void	load_identity();
		virtual void	push_transform();
		virtual void	pop_transform();

		virtual void	translate(float x, float y);
		virtual void	scale(float x, float y);
		virtual void	rotate(float degrees);

		virtual void	set_draw_color(Color c);

		virtual void	draw_arc(float circumf, float xr, float yr, int fine);
		virtual void	draw_arc_fill(float circumf, float xr, float yr, int fine);
		virtual void	draw_arc_line(float circumf, float xr, float yr, int fine);

		virtual void	draw_rect(float w, float h);
		virtual void	draw_rect_fill(float w, float h);
		virtual void	draw_rect_line(float w, float h);

		virtual void	draw_roundrect(float w, float h, float r, int fine);
		virtual void	draw_roundrect_fill(float w, float h, float r, int fine);
		virtual void	draw_roundrect_line(float w, float h, float r, int fine);

		virtual void	draw_line(float x1, float y1, float x2, float y2);
		virtual void	draw_lines(float vertices[], int n, bool loop);

		// TODO more GL properties (framebuffers?)

		virtual void	redraw();
	};
}

#endif
