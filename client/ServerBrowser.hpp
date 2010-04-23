/*
 * client/ServerBrowser.hpp
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

#ifndef LM_CLIENT_SERVERBROWSER_HPP
#define LM_CLIENT_SERVERBROWSER_HPP

//#include "GameController.hpp"
#include "GameWindow.hpp"
#include "TextManager.hpp"
#include "Font.hpp"
#include "ScrollArea.hpp"
#include "ScrollBar.hpp"
#include "common/misc.hpp"
#include "common/network.hpp"

namespace LM {
	class GameController;
	
	class ServerBrowser {
	private:
		const static int TEXT_LAYER;
		GameController& m_parent;
		GameWindow*	m_window;
		TextManager*	m_text_manager;
		Font*		m_font;
		Font*		m_medium_font;
		Font*		m_menu_font;
		int		m_screen_width;
		int		m_screen_height;
		TableBackground* m_background;
		TableBackground* m_selection;
		std::vector<TableBackground*> m_buttons;
		std::map<std::string, Text*> m_items;
		std::vector<IPAddress> m_server_list;
		int		m_server_list_count;
		int		m_selected_item;
		ScrollBar* 	m_scrollbar;
		ScrollArea*	m_scrollarea;
		bool		m_is_invisible;

	public:
		ServerBrowser(GameController& parent, GameWindow* window, TextManager* textmanager, int screenwidth, int screenheight, Font* standardfont, Font* mediumfont, Font* menufont);
		~ServerBrowser();
		void		set_visible(bool visible);
		void		clear();
		void		add_entry(IPAddress server_address, const std::string& current_map_name, int team_count[2], int max_players, uint64_t uptime, uint64_t ping, const std::string& server_name, const std::string& server_location);
		std::string	check_button_press(int mouse_x, int mouse_y);
		int		check_item_select(int mouse_x, int mouse_y);
		int		get_selected_item();
		void		scrollbar_button_event(const SDL_MouseButtonEvent& event);
		void		scrollbar_motion_event(const SDL_MouseMotionEvent& event);
		void		autoscroll(double scale);
		bool		is_invisible();
		int		get_count();
		void		set_count(int count);
		void		deselect();
		bool		contains_ip(IPAddress ip);
		IPAddress	get_server_info(int num);
	};
}

#endif
