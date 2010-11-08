/*
 * newclient/Client.hpp
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

#ifndef LM_NEWCLIENT_CLIENT_HPP
#define LM_NEWCLIENT_CLIENT_HPP

#include "client/ClientNetwork.hpp"

namespace LM {
	class Player;
	class Map;
	class Controller;
	class GameLogic;

	class Client {
	private:
		Controller* m_controller;
		GameLogic* m_logic;
		//ClientNetwork m_network;

		bool m_running;

	protected:
		// Networking, GameLogic calls, and base client updates are handled here
		void step(uint64_t diff);

		virtual const char* get_res_directory() const;

		virtual void add_player(Player* player);
		virtual void set_own_player(uint32_t id);
		virtual void remove_player(uint32_t id);

		Player* get_player(uint32_t id);
		GameLogic* get_game();

		virtual void set_map(Map* map);

	public:
		Client();
		virtual ~Client();
		virtual Player* make_player(const char* name, uint32_t id, char team);
		virtual Map* make_map();

		void set_controller(Controller* controller);

		void set_running(bool running);
		bool running() const;

		// Packet callbacks
		// TODO rename these packets
		virtual void begin_game(Map* map = 0);
		virtual void end_game();

		virtual void new_round(std::string map_name,
		                       int map_revision,
		                       int map_width,
		                       int map_height,
		                       bool round_started,
		                       uint64_t time_until_start);

		virtual void start_round();
		virtual void end_round();

		virtual void welcome(uint32_t player_id,
		                     std::string player_name,
		                     char team);

		virtual void announce(uint32_t player_id,
		                      std::string player_name,
		                      char team);
		// End packet callbacks

		// Main loop: override for subclass behaviors, but call step inside
		virtual void run();
	};
}

#endif
