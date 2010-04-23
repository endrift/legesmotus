/*
 * client/ListMenuItem.hpp
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

#ifndef LM_CLIENT_LISTMENUITEM_HPP
#define LM_CLIENT_LISTMENUITEM_HPP

#include "GraphicGroup.hpp"
#include "MenuItem.hpp"
#include <vector>

namespace LM {
	class TextManager;
	class ListMenuItem : public MenuItem, public FormItem {
	private:
		GraphicGroup		m_group;
		MenuItem*		m_reference;
		std::vector<MenuItem*>	m_options;
		unsigned int	m_default;
		unsigned int	m_current;

	protected:
		virtual void state_changed(State old_state, State new_state);

	public:
		ListMenuItem(std::string name, MenuItem* reference);
		virtual ~ListMenuItem();

		virtual std::string	get_value() const;
		virtual std::string	get_default_value() const;

		virtual const GraphicGroup*	get_graphic() const;
		virtual GraphicGroup*	get_graphic();

		virtual bool	is_mouse_over(int x, int y) const;

		void	add_option(MenuItem* option);
		void	remove_option(MenuItem* option);
		const MenuItem*	get_current_option() const;
		MenuItem*	get_current_option();
		const MenuItem*	get_default_option() const;
		MenuItem*	get_default_option();
		const MenuItem*	get_option(unsigned int index) const;
		MenuItem*	get_option(unsigned int index);
		unsigned int	size() const;

		void	set_current_index(unsigned int index);
		void	set_default_index(unsigned int index);

		unsigned int	get_current_index() const;
		unsigned int	get_default_index() const;

		virtual void	reset();
	};
}

#endif
