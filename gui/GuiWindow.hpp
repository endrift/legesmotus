/*
 * gui/GuiWindow.hpp
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

#ifndef LM_GUI_GUIWINDOW_HPP
#define LM_GUI_GUIWINDOW_HPP

#include "common/misc.hpp"
#include "Widget.hpp"
#include "Label.hpp"
#include "Font.hpp"

#include <string>

namespace LM {
	class GuiWindow : public Widget {
	private:
		Label	m_title_widget;

		Color	m_decoration_color;
		Color	m_title_color;
		Color	m_background_color;

		float	m_decoration_width;
		float	m_title_height;

		Widget*	m_widget;

		void	draw_decoration(DrawContext* ctx) const;

	public:
		GuiWindow(Widget* parent = NULL);

		void	set_title_text(const std::wstring& title);
		void	set_title_text_font(Font* font);

		float	get_viewport_width() const;
		float	get_viewport_height() const;

		void	set_main_widget(Widget* widget);

		virtual void set_width(float w);
		virtual void set_height(float h);

		virtual void draw(DrawContext* ctx) const;
	};
}

#endif
