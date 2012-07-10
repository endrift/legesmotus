/*
 * gui/ScrollingFrame.hpp
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

#include "BackgroundFrame.hpp"
#include "ScrollBar.hpp"

#ifndef LM_GUI_SCROLLINGFRAME_HPP
#define LM_GUI_SCROLLINGFRAME_HPP

namespace LM {
	class ScrollingFrame : public BackgroundFrame, public ScrollBarCallback {
	private:
		ScrollBar* m_bottom_scrollbar;
		ScrollBar* m_right_scrollbar;
		bool m_can_scroll_horiz;
		bool m_can_scroll_vert;
		std::map<int, Point> m_base_object_positions;
		
		virtual void private_mouse_clicked(bool child_handled, float x, float y, bool down, int button = 0);
		virtual void private_mouse_moved(bool child_handled, float x, float y, float delta_x, float delta_y);
		
	public:
		ScrollingFrame(Widget* parent);
		virtual ~ScrollingFrame();
		
		virtual void set_width(float width);
		virtual void set_height(float height);
		
		float get_usable_width() const;
		float get_usable_height() const;
		
		virtual Point get_relative_point(float x, float y);
		
		void set_bar_minor_dimension(float size);
		void set_bar_border_radius(float size);
		void set_bar_padding(float padding);
		void set_bar_arrow_size(float size);
		void set_bar_arrow_minor_axis(float size);
		void set_bar_arrow_major_axis(float size);
		void set_bar_arrow_color(const Color& color);
		void set_bar_scroll_speed(float pixels_per_millisecond);
		
		void set_bar_color(const Color& c, ColorType type = COLOR_PRIMARY);
		void set_bar_inactive_color(const Color& c, ColorType type = COLOR_PRIMARY);
		void set_bar_hover_color(const Color& c, ColorType type = COLOR_PRIMARY);
		void set_bar_pressed_color(const Color& c, ColorType type = COLOR_PRIMARY);
		
		void set_bar_change_on_pressed(bool change);
		void set_bar_change_on_hover(bool change);
		
		void set_can_scroll(bool can_scroll_horiz, bool can_scroll_vert);
		
		virtual void scroll_fraction_changed(int bar_id, ScrollBar* scroll_bar, float fraction);
	
		virtual void update(uint64_t timediff);
		virtual void draw(DrawContext* ctx) const;
		virtual void draw_internals(DrawContext* ctx) const;
	};
}

#endif
