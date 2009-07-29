/*
 * client/MenuItem.hpp
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

#ifndef LM_CLIENT_MENUITEM_HPP
#define LM_CLIENT_MENUITEM_HPP

#include <string>

namespace LM {
	class Graphic;
	class MenuItem {
	public:
		enum State {
			NORMAL,
			STATIC,
			HOVER,
			CLICKED,
			DISABLED
		};
	private:
		std::string	m_value;
		State		m_state;

	protected:
		virtual void state_changed(State old_state, State new_state) {}

	public:
		MenuItem(std::string value, State state = NORMAL);
		virtual ~MenuItem() {}

		void		set_state(State state);
		void		set_value(std::string value);

		State		get_state() const;
		std::string	get_value() const;
		virtual const Graphic*	get_graphic() const = 0;
		virtual Graphic*	get_graphic() = 0;

		bool		disabled() const;

		virtual bool	is_mouse_over(int x, int y) const;
	};
}

#endif
