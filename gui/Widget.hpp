/*
 * gui/Widget.hpp
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

#ifndef LM_GUI_WIDGET_HPP
#define LM_GUI_WIDGET_HPP

#include <map>
#include <boost/signals2/signal.hpp>
// TODO #include "common/Point.hpp"
#include "DrawContext.hpp"

namespace LM {
	class Widget {
	private:
		Widget* m_parent;
		std::multimap<int, Widget*> m_children;

		float	m_x;
		float	m_y;
		float	m_w;
		float	m_h;

	public:
		Widget(Widget* parent = NULL);
		virtual ~Widget();

		void	set_parent(Widget* new_parent);
		Widget*	get_parent();
		void	add_child(Widget* child, int priority = 0);
		void	remove_child(Widget* child);
		void	clear_children();
		const std::multimap<int, Widget*>& get_children();

		Widget*	top_child_at(float x, float y);
		std::multimap<int, Widget*> children_at(float x, float y);
		virtual bool contains_point(float x, float y);

		void change_priority(int old_priority, int new_priority);
		void change_priority(Widget* widget, int new_priority);

		virtual void set_x(float x);
		virtual void set_y(float y);
		virtual void set_width(float w);
		virtual void set_height(float h);

		float	get_x() const;
		float	get_y() const;
		float	get_width() const;
		float	get_height() const;

		virtual void focus();
		virtual void blur();

		virtual void mouse_clicked(float x, float y, bool down, int button = 0);
		virtual void mouse_moved(float x, float y, float delta_x, float delta_y);
		virtual void keypress(int key, bool down);

		virtual void draw(DrawContext* ctx) const;

		boost::signals2::signal<void ()> s_focus;
		boost::signals2::signal<void ()> s_blur;

		boost::signals2::signal<void (float, float, bool, int)> s_mouse_clicked;
		boost::signals2::signal<void (float, float, float, float)> s_mouse_moved;
		boost::signals2::signal<void (int, bool)> s_keypress;
	};
}

#endif
