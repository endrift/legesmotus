/*
 * client/TextMenuItem.hpp
 *
 * This file is part of Leges Motus, a networked, 2D shooter set in zero gravity.
 * 
 * Copyright 2009 Andrew Ayer, Nathan Partlan, Jeffrey Pfau
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

#ifndef LM_CLIENT_TEXTMENUITEM_HPP
#define LM_CLIENT_TEXTMENUITEM_HPP

#include "common/misc.hpp"
#include "MenuItem.hpp"

namespace LM {
	class Text;
	class TextManager;
	class Graphic;
	// TODO move default colors from GameController to here
	class TextMenuItem : public MenuItem {
	private:
		Text*	m_text;
		Color	m_plain_fg;
		Color	m_plain_bg;
		Color	m_hover_fg;
		Color	m_hover_bg;
		Color	m_disabled_fg;
		Color	m_disabled_bg;

	protected:
		virtual void state_changed(State old_state, State new_state);

	public:
		TextMenuItem(Text* text, std::string value, State state = NORMAL);
		static TextMenuItem* with_manager(TextManager* manager, std::string text,
			std::string value, double x, double y, State state = NORMAL);

		void	set_plain_fg_color(const Color& color);
		void	set_plain_bg_color(const Color& color);
		void	set_hover_fg_color(const Color& color);
		void	set_hover_bg_color(const Color& color);
		void	set_disabled_fg_color(const Color& color);
		void	set_disabled_bg_color(const Color& color);

		virtual	const Graphic*	get_graphic() const;
		virtual	Graphic*	get_graphic();
	};
}

#endif
