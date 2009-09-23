/*
 * client/GraphicMenuItem.hpp
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

#ifndef LM_CLIENT_GRAPHICMENUITEM_HPP
#define LM_CLIENT_GRAPHICMENUITEM_HPP

#include "Graphic.hpp"
#include "MenuItem.hpp"

namespace LM {
	class GraphicMenuItem : public MenuItem {
	public:
		static const Color PLAIN_COLOR;
		static const Color HOVER_COLOR;
		static const Color DISABLED_COLOR;
	private:
		Graphic*	m_graphic;
		Color	m_plain;
		Color	m_hover;
		Color	m_disabled;

	protected:
		virtual void state_changed(State old_state, State new_state);

	public:
		GraphicMenuItem(Graphic* graphic, std::string value, State state = NORMAL);

		void	set_plain_color(const Color& color);
		void	set_hover_color(const Color& color);
		void	set_disabled_color(const Color& color);

		void	set_graphic(Graphic* graphic);
		virtual	const Graphic*	get_graphic() const;
		virtual	Graphic*	get_graphic();
	};
}

#endif
