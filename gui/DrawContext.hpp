/*
 * gui/DrawContext.hpp
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

#ifndef LM_GUI_DRAWCONTEXT_HPP
#define LM_GUI_DRAWCONTEXT_HPP

#include "common/misc.hpp"
#include <vector>

namespace LM {
	class Widget;

	class DrawContext {
	public:
		typedef unsigned int Image;
		enum PixelFormat {
			RGBA,
			ALPHA
		};

	private:
		std::vector<Widget*> m_widgets;
		int		m_focusIndex;

	protected:
		/*std::vector<Widget*>::iterator	begin();
		std::vector<Widget*>::iterator	end();
		virtual void	focus_change(int oldFocus, int newFocus);*/

	public:
		virtual ~DrawContext();

		/*void	add_widget(Widget* widget, int index = -1);
		void	remove_widget(int index);
		void	remove_widget(Widget* widget);
		Widget*	get_widget(int index);
		int		num_widgets() const;

		void	mouse_click(int x, int y, bool down, int button = 0);
		void	mouse_move(int x, int y);
		void	keypress(int key, bool down);*/

		virtual int		get_width() const = 0;
		virtual int		get_height() const = 0;

		virtual void	load_identity() = 0;
		virtual void	push_transform() = 0;
		virtual void	pop_transform() = 0;
		virtual void	clip() = 0;
		virtual void	unclip() = 0;
		virtual void	finish_clip() = 0;
		virtual int		clip_depth() = 0;

		// TODO offsets (translate -> scale -> rotate?, adustable order?)

		virtual void	translate(float x, float y) = 0;
		virtual void	scale(float x, float y) = 0;
		virtual void	rotate(float degrees) = 0;

		virtual void	set_draw_color(Color c) = 0;

		virtual void	draw_arc(float circumf, float xr, float yr, int fine) = 0;
		virtual void	draw_arc_fill(float circumf, float xr, float yr, int fine) = 0;
		virtual void	draw_arc_line(float circumf, float xr, float yr, int fine) = 0;

		virtual void	draw_rect(float w, float h) = 0;
		virtual void	draw_rect_fill(float w, float h) = 0;
		virtual void	draw_rect_line(float w, float h) = 0;

		virtual void	draw_roundrect(float w, float h, float r, int fine);
		virtual void	draw_roundrect_fill(float w, float h, float r, int fine);
		virtual void	draw_roundrect_line(float w, float h, float r, int fine);

		virtual void	draw_line(float x1, float y1, float x2, float y2) = 0;
		virtual void	draw_lines(float vertices[], int n, bool loop) = 0;

		virtual Image	gen_image(int* width, int* height, PixelFormat format, unsigned char* data) = 0;
		virtual void	del_image(Image img) = 0;

		virtual void	draw_image(int width, int height, Image img) = 0;
		virtual void	bind_image(Image img) = 0;
		virtual void	draw_bound_image(int width, int height) = 0;

		virtual void	redraw() = 0;
	};
}

#endif
