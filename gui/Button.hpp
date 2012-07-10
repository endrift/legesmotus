/*
 * gui/Button.hpp
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

#ifndef LM_GUI_BUTTON_HPP
#define LM_GUI_BUTTON_HPP

#include "BackgroundFrame.hpp"
#include "Widget.hpp"

namespace LM {
	class Label;
	class Font;

	class ButtonCallback {
		public:
			virtual ~ButtonCallback() {};
		
			virtual void button_pressed(int button_id, std::string caption) = 0;
			virtual void button_released(int button_id, std::string caption) = 0;
	};

	class Button : public BackgroundFrame {
	public:
		enum ButtonState {
			STATE_NORMAL,
			STATE_INACTIVE,
			STATE_PRESSED
		};
		
	private:
		Label* m_caption;
		std::string m_caption_string;
		ButtonCallback* m_callback_object;
		
		Color m_normal_colors[COLOR_MAX];
		Color m_inactive_colors[COLOR_MAX];
		Color m_hover_colors[COLOR_MAX];
		Color m_pressed_colors[COLOR_MAX];
		
		bool m_change_on_hover;
		
		bool m_change_on_mousedown;
		
		bool m_is_hovered;
		
		ButtonState m_state;

	public:
	
		Button(Widget* parent = NULL);
		virtual ~Button();
		
		virtual void set_callback(ButtonCallback* callback);
		
		virtual void set_label(std::string caption, Font* font);
		
		virtual void private_mouse_clicked(bool child_handled, float x, float y, bool down, int button = 0);
		virtual void private_mouse_moved(bool child_handled, float x, float y, float delta_x, float delta_y);
		
		void set_normal_color(const Color& c, ColorType type = COLOR_PRIMARY);
		const Color& get_normal_color(ColorType type = COLOR_PRIMARY) const;
		
		void set_inactive_color(const Color& c, ColorType type = COLOR_PRIMARY);
		const Color& get_inactive_color(ColorType type = COLOR_PRIMARY) const;
		
		void set_hover_color(const Color& c, ColorType type = COLOR_PRIMARY);
		const Color& get_hover_color(ColorType type = COLOR_PRIMARY) const;
		
		void set_pressed_color(const Color& c, ColorType type = COLOR_PRIMARY);
		const Color& get_pressed_color(ColorType type = COLOR_PRIMARY) const;
		
		void update_colors();
		
		void set_state(ButtonState state);
		ButtonState get_state() const;
		
		void set_change_on_hover(bool change);
		void set_change_on_pressed(bool change);
	};
}

#endif
