/*
 * gui/InputDriver.hpp
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

#ifndef LM_GUI_INPUTDRIVER_HPP
#define LM_GUI_INPUTDRIVER_HPP

#include "common/Point.hpp"
#include "input.hpp"

#include <queue>

namespace LM {
	class InputDriver {
	private:
		std::queue<KeyEvent> m_key_events;
		std::queue<MouseMotionEvent> m_motion_events;
		std::queue<MouseButtonEvent> m_button_events;

	protected:
		void register_event(const KeyEvent& event);
		void register_event(const MouseMotionEvent& event);
		void register_event(const MouseButtonEvent& event);

	public:
		virtual ~InputDriver();

		virtual int update() = 0;

		virtual void mouse_position(int* x, int* y) const = 0;
		virtual int mouse_buttons() const = 0;

		bool poll_keys(KeyEvent* event);
		bool poll_mouse_motion(MouseMotionEvent* event);
		bool poll_mouse_buttons(MouseButtonEvent* event);
	};
}

#endif
