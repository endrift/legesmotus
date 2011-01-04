/*
 * gui/HumanController.hpp
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

#ifndef LM_GUI_HUMANCONTROLLER_HPP
#define LM_GUI_HUMANCONTROLLER_HPP

#include "newclient/Controller.hpp"
#include "InputSink.hpp"
#include "Bindings.hpp"
#include <string>

namespace LM {
	class HumanController : public InputSink, public Controller {
	private:
		// We need to swap changesets so we can clear one during update
		int m_changes[2];
		int m_changeset;

		int m_mouse_x;
		int m_mouse_y;
		int m_view_w;
		int m_view_h;

		int m_weapon;
		int m_nextweapon;
		int m_lastweapon;
		int m_wdelta;

		Bindings m_bindings;

		bool m_typing_message;
		std::wstring m_message;
		bool m_message_is_team_only[2];

		void process_control(const Bindings::ControlEvent& event);

	public:
		HumanController();

		void set_viewport_size(int w, int h);

		virtual void key_pressed(const KeyEvent& event);
		virtual void mouse_moved(const MouseMotionEvent& event);
		virtual void mouse_clicked(const MouseButtonEvent& event);
		virtual void system_event(const SystemEvent& event);

		virtual void update(uint64_t diff, const GameLogic& state);

		virtual int get_changes() const;
		virtual float get_aim() const;
		virtual float get_distance() const;
		virtual int get_weapon() const;

		virtual std::wstring get_message() const;
		virtual bool message_is_team_only() const;
		virtual void received_message(const Player* p, const std::wstring& message);
	};
}

#endif
