/*
 * gui/TextInput.hpp
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

#include "BackgroundFrame.hpp"

#ifndef LM_GUI_TEXTINPUT_HPP
#define LM_GUI_TEXTINPUT_HPP

namespace LM {
	class Label;
	class Font;
	class TextInput;

	class TextInputCallback {
		public:
			virtual ~TextInputCallback() {};
		
			virtual void input_box_clicked(int box_id, TextInput* input_box, std::wstring caption) = 0;
			virtual void input_completed(int box_id, TextInput* input_box, std::wstring caption) = 0;
			virtual void input_changed(int box_id, TextInput* input_box, std::wstring caption) = 0;
	};

	class TextInput : public BackgroundFrame {
	private:
		Label* m_caption;
		std::wstring m_caption_string;
		TextInputCallback* m_callback_object;
		
		Color m_cursor_color;
		
		uint64_t m_cursor_blink_millis;
		float m_cursor_transparency;
		
		bool m_mouse_down;
		bool m_is_focused;
	
		virtual void private_mouse_clicked(bool child_handled, float x, float y, bool down, int button = 0);
		virtual void private_keypress(const KeyEvent& event);
	public:
		TextInput(Widget* parent = NULL);
		virtual ~TextInput();

		virtual void set_font(Font* font);
		virtual void set_label(const std::wstring& caption);
		
		virtual void set_callback(TextInputCallback* callback);
		
		virtual void set_cursor_color(const Color& color);
		virtual void set_cursor_blink_millis(uint64_t millis);
		
		virtual void update(uint64_t timediff);
		
		virtual void draw_extras(DrawContext* ctx) const;
	};
}

#endif
