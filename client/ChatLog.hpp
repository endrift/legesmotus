/*
 * client/ChatLog.hpp
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

#ifndef LM_CLIENT_CHATLOG_HPP
#define LM_CLIENT_CHATLOG_HPP

#include "GameWindow.hpp"
#include "TextManager.hpp"
#include "Font.hpp"
#include "ScrollArea.hpp"
#include "ScrollBar.hpp"
#include "common/misc.hpp"
#include "common/network.hpp"

namespace LM {
	class GameController;	
	
	class ChatLog {
	private:
		const static int TEXT_LAYER;
		const static int LINE_SPACING;
		const static int RIGHT_PADDING;
		GameController& m_parent;
		GameWindow*	m_window;
		TextManager*	m_text_manager;
		Font*		m_font;
		Font*		m_medium_font;
		Font*		m_menu_font;
		int		m_screen_width;
		int		m_screen_height;
		TableBackground* m_background;
		ScrollBar* 	m_scrollbar;
		ScrollArea*	m_scrollarea;
		Text*		m_title;
		bool		m_is_invisible;
		std::vector<Text*> m_items;
	
	public:
		ChatLog(GameController& parent, GameWindow* window, TextManager* textmanager, int screenwidth, int screenheight, Font* standardfont, Font* mediumfont, Font* menufont);
		~ChatLog();
		void		set_visible(bool visible);
		bool		is_invisible();
		void		delete_message(int num);
		void		add_message(std::string message, Color color, Color shadow);
		void		scrollbar_button_event(const SDL_MouseButtonEvent& event);
		void		scrollbar_motion_event(const SDL_MouseMotionEvent& event);
		void		autoscroll(double scale);
	};
}

#endif
