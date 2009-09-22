/*
 * client/Menu.hpp
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

#ifndef LM_CLIENT_MENU_HPP
#define LM_CLIENT_MENU_HPP

#include "MenuItem.hpp"
#include "ClientSDL.hpp"
#include "GraphicGroup.hpp"
#include <string>
#include <vector>

namespace LM {
	class Menu {
	private:
		MenuItem*		m_over;
		MenuItem*		m_pressed;
		GraphicGroup	m_group;

	protected:
		std::vector<MenuItem*>	m_menu_items;

		virtual void mouseover(MenuItem* item, int x, int y) { (void)(item); }
		virtual void mouseout(MenuItem* item, int x, int y) { (void)(item); }
		virtual void mousedown(MenuItem* item, int x, int y) { (void)(item); }
		virtual void mouseup(MenuItem* item, int x, int y) { (void)(item); }

		void	add_item_internal(MenuItem* item);
		void	remove_item_internal(MenuItem* item); // Note: does not delete the item

	public:
		Menu();
		virtual ~Menu();

		GraphicGroup*	get_graphic_group();

		MenuItem*		mouse_motion_event(const SDL_MouseMotionEvent& event);
		MenuItem*		mouse_button_event(const SDL_MouseButtonEvent& event);
		virtual MenuItem* item_at_position(int x, int y) = 0;
	};
}

#endif
