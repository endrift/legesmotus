/*
 * gui/Widget.hpp
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

#ifndef LM_GUI_WIDGET_HPP
#define LM_GUI_WIDGET_HPP

#include <map>
// TODO #include "common/Point.hpp"
#include "DrawContext.hpp"
#include "pubsub.hpp"
#include "common/Iterator.hpp"
#include "input.hpp"
#include <stdint.h>

namespace LM {
	class Widget : public Publisher {
	public:
		enum ColorType {
			COLOR_PRIMARY,
			COLOR_SECONDARY,

			COLOR_MAX
		};

	private:
		Widget* m_parent;
		std::multimap<int, Widget*> m_children;
		
		static uint64_t CURR_ID;

		float m_x;
		float m_y;
		float m_w;
		float m_h;
		
		uint64_t m_id;

		bool m_drawable;
		bool m_receive_input;

		Color m_colors[COLOR_MAX];
		
		virtual void on_add_child(Widget* child, int priority);
		virtual void on_remove_child(Widget* child);
		
		virtual void private_mouse_clicked(bool child_handled, float x, float y, bool down, int button = 0);
		virtual void private_mouse_moved(bool child_handled, float x, float y, float delta_x, float delta_y);
		virtual void private_keypress(const KeyEvent& event);
	public:
		Widget(Widget* parent = NULL);
		virtual ~Widget();

		void	set_parent(Widget* new_parent);
		Widget*	get_parent();
		void	add_child(Widget* child, int priority = 0);
		void	remove_child(Widget* child);
		void	clear_children();
		Iterator<std::pair<int, Widget*> > list_children();
		
		uint64_t get_id() const;
		void set_id(uint64_t id);

		void set_receives_input(bool receive_input);
		bool receives_input() const;

		virtual Point get_relative_point(float x, float y);
		Widget*	top_child_at(float x, float y);
		std::multimap<int, Widget*> children_at(float x, float y);
		virtual bool contains_point(float x, float y) const;

		void change_priority(int old_priority, int new_priority);
		void change_priority(Widget* widget, int new_priority);

		virtual void set_x(float x);
		virtual void set_y(float y);
		virtual void set_width(float w);
		virtual void set_height(float h);

		float get_x() const;
		float get_y() const;
		float get_width() const;
		float get_height() const;
		
		float get_absolute_x() const;
		float get_absolute_y() const;

		void set_color(const Color& c, ColorType type = COLOR_PRIMARY);
		// Caution: if set_color is used, the reference will be updated to the new color
		const Color& get_color(ColorType type = COLOR_PRIMARY) const;

		virtual void set_drawable(bool drawable);
		bool is_drawable() const;

		void focus();
		void blur();

		void mouse_clicked(float x, float y, bool down, int button = 0);
		void mouse_moved(float x, float y, float delta_x, float delta_y);
		void keypress(const KeyEvent& event);
		
		virtual void update(uint64_t timediff);

		virtual void draw(DrawContext* ctx) const;
		
		virtual void draw_internals(DrawContext* ctx) const;
	};
}

#endif
