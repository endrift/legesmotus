/*
 * gui/Bindings.hpp
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

#ifndef LM_GUI_BINDINGS_HPP
#define LM_GUI_BINDINGS_HPP

#include "input.hpp"

namespace LM {
	class Bindings {
	public:
		enum ControlType {
			CONTROL_NONE = 0,
			CONTROL_JUMP,
			CONTROL_SET_WEAPON,
			CONTROL_INCREMENT_WEAPON,
			CONTROL_DECREMENT_WEAPON,
			CONTROL_FIRE,
			CONTROL_STOP_FIRE,
			CONTROL_BEGIN_TYPING
		};

		struct ControlEvent {
			ControlType type;
			union {
				struct {
					bool is_team_only;
				} typing;
			};
		};

	private:
		// TODO store bindings here

	public:
		ControlEvent process_event(const KeyEvent& event);
		ControlEvent process_event(const MouseMotionEvent& event);
		ControlEvent process_event(const MouseButtonEvent& event);
	};
}

#endif
