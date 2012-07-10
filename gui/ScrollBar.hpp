/*
 * gui/ScrollBar.hpp
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

#include "Widget.hpp"

#ifndef LM_GUI_SCROLLBAR_HPP
#define LM_GUI_SCROLLBAR_HPP

namespace LM {
	class ScrollBar;

	class ScrollBarCallback {
		public:
			virtual ~ScrollBarCallback() {};
		
			virtual void scroll_fraction_changed(int scrollbar_id, ScrollBar* bar, float fraction) = 0;
	};

	class ScrollBar : public Widget {
	public:
		enum ScrollBarState {
			STATE_NORMAL,
			STATE_INACTIVE,
			STATE_TOP_PRESSED,
			STATE_BOTTOM_PRESSED,
			STATE_MIDDLE_DRAGGED
		};
	
	private:
		Color m_hover_colors[COLOR_MAX];
		Color m_inactive_colors[COLOR_MAX];
		Color m_pressed_colors[COLOR_MAX];
		
		bool m_is_horizontal;
		bool m_use_parent_size;
		float m_parent_size;
		float m_virtual_size;
		float m_scroll_fraction;
		float m_border_radius;
		float m_border_padding;
		int m_max_corner_sections;
		float m_arrow_icon_major_axis;
		float m_arrow_icon_minor_axis;
		float m_arrow_size;
		
		Point m_last_mouse_pos;
		
		float m_drag_start_pos;
		float m_drag_start_percent;
		
		float m_scroll_pixels_per_milli;
		
		ScrollBarState m_state;
		
		Color m_arrow_color;

		bool m_change_on_hover;
		bool m_change_on_mousedown;
		
		ScrollBarCallback* m_callback_object;
		
		bool point_is_in_top(float x, float y) const;
		bool point_is_in_drag_bar(float x, float y) const;
		bool point_is_in_bottom(float x, float y) const;
		
		float get_internal_size() const;
		float get_drag_bar_length() const;
		float get_max_scroll_fraction() const;
		float get_drag_bar_center() const;
	public:
		ScrollBar(Widget* parent = NULL);
		virtual ~ScrollBar();
		
		virtual float get_parent_size_transform() const;
		virtual float get_scroll_fraction() const;
		virtual float get_virtual_size() const;
		virtual bool is_horizontal() const;
		
		void set_inactive_color(const Color& c, ColorType type = COLOR_PRIMARY);
		const Color& get_inactive_color(ColorType type = COLOR_PRIMARY) const;
		
		void set_hover_color(const Color& c, ColorType type = COLOR_PRIMARY);
		const Color& get_hover_color(ColorType type = COLOR_PRIMARY) const;
		
		void set_pressed_color(const Color& c, ColorType type = COLOR_PRIMARY);
		const Color& get_pressed_color(ColorType type = COLOR_PRIMARY) const;
		
		void set_state(ScrollBarState state);
		ScrollBarState get_state() const;
		
		void set_change_on_pressed(bool change);
		void set_change_on_hover(bool change);
		
		void set_use_parent_size(bool use_parent_size);
		
		virtual void set_callback_object(ScrollBarCallback* m_callback_object);
		
		virtual void set_horizontal(bool horizontal);
		
		virtual void set_virtual_size(float size);
		
		virtual void set_parent_size(float size);
		
		virtual void set_scroll_fraction(float fraction);
		
		virtual void set_border_radius(float radius);
		
		virtual void set_border_padding(float padding);
		
		virtual void set_border_max_roundness(int sections);
		
		virtual void set_arrow_size(float pixels);
		
		virtual void set_arrow_icon_major_axis(float pixels);
		virtual void set_arrow_icon_minor_axis(float pixels);
		
		virtual void set_scroll_speed(float pixels_per_millisecond);
		
		virtual void set_arrow_color(const Color& color);
		
		virtual void private_mouse_clicked(bool child_handled, float x, float y, bool down, int button = 0);
		virtual void private_mouse_moved(bool child_handled, float x, float y, float delta_x, float delta_y);
		
		virtual void update(uint64_t timediff);
		virtual void draw(DrawContext* ctx) const;
	};
}

#endif
