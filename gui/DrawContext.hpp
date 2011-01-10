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
	class Image;
	class Widget;
	class ResourceCache;
	class ShaderSet;

	typedef unsigned int PixelShader;

	class DrawContext {
	public:
		typedef unsigned int Image;

		enum PixelFormat {
			RGBA,
			ALPHA
		};

		enum BlendMode {
			BLEND_NORMAL,
			BLEND_ADD,
			BLEND_MULTIPLY,
			BLEND_SCREEN
		};

	private:
		Widget*	m_root_widget;
		bool	m_dirtied;

	protected:
		/*std::vector<Widget*>::iterator	begin();
		std::vector<Widget*>::iterator	end();
		virtual void	focus_change(int oldFocus, int newFocus);*/

	public:
		DrawContext();
		virtual ~DrawContext();

		void	set_root_widget(Widget* widget);
		Widget*	get_root_widget();

		/*void	mouse_click(int x, int y, bool down, int button = 0);
		void	mouse_move(int x, int y);
		void	keypress(int key, bool down);*/

		void	dirty();
		void	clean();
		bool	is_dirty();

		virtual void	make_active() = 0;
		// Get this DrawContext as an image
		virtual LM::Image get_image(const std::string& name, ResourceCache* cache) = 0;

		virtual int		get_width() const = 0;
		virtual int		get_height() const = 0;

		virtual void	set_active_camera() = 0;
		virtual void	set_active_graphics() = 0;

		virtual void	load_identity() = 0;
		virtual void	push_transform() = 0;
		virtual void	pop_transform() = 0;

		virtual void	start_clip() = 0;
		virtual void	clip_add() = 0;
		virtual void	clip_sub() = 0;
		virtual void	finish_clip() = 0;
		virtual void	invert_clip() = 0;
		virtual void	push_clip() = 0;
		virtual void	pop_clip() = 0;
		virtual int		clip_depth() = 0;

		// TODO offsets (translate -> scale -> rotate?, adustable order?)

		virtual void	translate(float x, float y) = 0;
		virtual void	scale(float x, float y) = 0;
		virtual void	rotate(float degrees) = 0;
		virtual void	skew_x(float amount) = 0;
		virtual void	skew_y(float amount) = 0;

		virtual void	set_draw_color(Color c) = 0;
		virtual void	set_blend_mode(BlendMode m) = 0;

		virtual PixelShader load_pixel_shader(const std::string& filename) = 0;
		virtual void	delete_pixel_shader(PixelShader shader) = 0;

		virtual ShaderSet* create_shader_set() = 0;
		virtual void	bind_shader_set(ShaderSet* shaders) = 0;
		virtual void	unbind_shader_set() = 0;

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
		virtual void	draw_lines(const float vertices[], int n, bool loop) = 0;
		virtual void	draw_polygon(const float vertices[], int n) = 0;
		virtual void	draw_polygon_fill(const float vertices[], int n) = 0;

		virtual Image	gen_image(int* width, int* height, PixelFormat format, const unsigned char* data) = 0;
		virtual void	add_mipmap(Image handle, int level, int* width, int* height, PixelFormat format, const unsigned char* data) = 0;
		virtual void	del_image(Image img) = 0;

		virtual void	bind_image(Image img) = 0;
		virtual void	unbind_image() = 0;
		virtual void	draw_image(int width, int height, Image img) = 0;
		virtual void	draw_bound_image(int width, int height) = 0;
		virtual void	draw_bound_image_region(int width, int height,
												float tex_x, float tex_y,
												float tex_width, float tex_height) = 0;
		virtual void	draw_bound_image_tiled(int width, int height,
											   float tex_x, float tex_y,
											   float tex_width, float tex_height) = 0;

		virtual void	clear() = 0;
		virtual void	redraw() = 0;
	};
}

#endif
