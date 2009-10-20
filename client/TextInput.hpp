/*
 * client/TextInput.hpp
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

#ifndef LM_CLIENT_TEXTINPUT_HPP
#define LM_CLIENT_TEXTINPUT_HPP

#include "FormItem.hpp"
#include "ScrollArea.hpp"
#include "Text.hpp"
#include "TableBackground.hpp"
#include "MenuItem.hpp"
#include <string>

namespace LM {
	class TextManager;
	class TextInput : public FormItem {
	private:
		std::string	m_current;
		std::string	m_prefix;
		std::string	m_default;
		TextManager*	m_manager;
		Graphic*	m_background;
		bool		m_bg_scale;
		double		m_bg_padding;
		Text*		m_current_sprite;
		Text*		m_prefix_sprite;
		ScrollArea*	m_crop_area;
		double		m_crop_width;
		GraphicGroup*	m_group;
		int		m_current_char;
		TableBackground* m_cursor;
		unsigned int	m_limit;
		GameWindow*	m_window;
		double		m_x;
		double		m_y;

		void	recalc();

	public:
		TextInput(TextManager* manager, double x, double y, unsigned int limit = 0);
		virtual ~TextInput();

		bool	mouse_button_event(const SDL_MouseButtonEvent& event);
		void	keyboard_event(const SDL_KeyboardEvent& event);

		void	set_value(const std::string& value);
		void	set_default_value(const std::string& value);
		void	set_prefix(const std::string& value);
		void	set_limit(unsigned int limit);
		void	set_crop_width(double width);
		void	remove_cropping();

		void	set_window(GameWindow* window);
		void	set_background(Graphic* background);
		void	set_background_scale(bool enable);
		void	set_background_padding(double padding);
		void	set_x(double x);
		void	set_y(double y);

		virtual std::string	get_value() const;
		virtual std::string	get_default_value() const;
		virtual void	reset();
	};
}

#endif
