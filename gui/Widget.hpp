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
// TODO #include "common/Point.hpp"
#include "DrawContext.hpp"

#define LM_DECLARE_LISTENER(type) extern const char* LISTENER_ ## type
#define LM_DEFINE_LISTENER(type) const char* LISTENER_ ## type = "LISTENER:" # type
#define LM_ASSIGN_LISTENER(l, type) ((l)->p = LISTENER_ ## type)
#define LM_SWITCH_LISTENER(l) ((l)->id)

namespace LM {
	class Widget {
	public:
		union ListenType {
			long id;
			const char* p;
		};

		class Listener {
		public:
			virtual ~Listener() {}
			virtual void handle(ListenType type, Widget* speaker, void* data) = 0;
		};

	private:
		Widget* m_parent;
		std::multimap<int, Widget*> m_children;
		std::map<long, std::pair<Listener*, void*> > m_listeners;

		float m_x;
		float m_y;
		float m_w;
		float m_h;

		bool m_drawable;

	protected:
		void raise(ListenType type);

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

		float get_x() const;
		float get_y() const;
		float get_width() const;
		float get_height() const;

		virtual void set_drawable(bool drawable);
		bool is_drawable() const;

		void focus();
		void blur();

		void mouse_clicked(float x, float y, bool down, int button = 0);
		void mouse_moved(float x, float y, float delta_x, float delta_y);
		void keypress(int key, bool down);

		virtual void draw(DrawContext* ctx) const;

		void set_listener(ListenType type, Listener* listener, void* data);
	};
}

#endif
