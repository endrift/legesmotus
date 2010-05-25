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

#include <list>

namespace LM {
	class Widget {
	private:
		Widget* m_parent;
		std::list<Widget*> m_children;

		float m_x;
		float m_y;
		float m_w;
		float m_h;

	protected:
		void add_child(Widget* child);

	public:
		Widget(Widget* parent = NULL);
		virtual ~Widget();

		void	set_parent(Widget* new_parent);
		void	get_parent();
		std::list<Widget*> get_children();

		void	child_at(float x, float y);

		virtual void set_x();
		virtual void set_y();
		virtual void set_width();
		virtual void set_height();

		float	get_x() const;
		float	get_y() const;
		float	get_width() const;
		float	get_height() const;

		virtual void focus();
		virtual void blur();

		virtual void mouse_clicked(float x, float y, bool down, int button = 0);
		virtual void mouse_moved(float x, float y, float delta_x, float delta_y);
		virtual void keypress(int key, bool down);

		virtual void redraw();
	};
}

#endif
