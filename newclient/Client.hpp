/*
 * newclient/Client.hpp
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

#ifndef LM_NEWCLIENT_CLIENT_HPP
#define LM_NEWCLIENT_CLIENT_HPP

#include "ClientNetwork.hpp"
#include "common/Packet.hpp"

namespace LM {
	class Player;
	class Map;
	class Controller;
	class GameLogic;
	class Weapon;

	class Client : public PacketReceiver {
	private:
		Controller* m_controller;
		GameLogic* m_logic;
		uint32_t m_player_id;
		ClientNetwork m_network;
		long m_curr_weapon;

		bool m_running;
		
		bool m_engaging_gate;
		bool m_jumping;

	protected:
		// Networking, GameLogic calls, and base client updates are handled here
		uint64_t step(uint64_t diff);

		virtual const char* get_res_directory() const;

		virtual void add_player(Player* player);
		virtual void set_own_player(uint32_t id);
		void remove_player(uint32_t id);
		virtual void remove_player(uint32_t id, const std::string& reason);

		Player* get_player(uint32_t id);
		
		void update_gates();
		void attempt_firing();
		void check_player_hits();
		
		void generate_player_update(uint32_t id, Packet* p);
		void generate_weapon_fired(uint32_t weapon_id, uint32_t player_id);
		void generate_gate_update(uint32_t player_id, char team, bool holding);
		void generate_player_died(uint32_t died_id, uint32_t killer_id, bool killer_is_player);

		GameLogic* get_game();
		Weapon* get_curr_weapon();
		uint32_t get_curr_weapon_id() const { return m_curr_weapon; };

		virtual void set_map(Map* map);

		virtual void round_init(Map* map);
		virtual void round_cleanup();
		
		void send_quit();

	public:
		Client();
		virtual ~Client();
		
		virtual Player* make_player(const char* name, uint32_t id, char team);
		virtual Map* make_map();
		virtual Weapon* make_weapon(WeaponReader& weapon_data);

		void set_controller(Controller* controller);
		virtual void set_curr_weapon(uint32_t id);

		void set_running(bool running);
		bool running() const;

		void connect(const IPAddress& server_address);

		// Packet callbacks
		// TODO rename these packets
		// Please try and keep these in packet order
		virtual void player_update(const Packet& p);
		virtual void weapon_discharged(const Packet& p);
		virtual void player_hit(const Packet& p);
		//virtual void message(const Packet& p);
		virtual void new_round(const Packet& p);
		virtual void round_over(const Packet& p);
		//virtual void score_update(const Packet& p);
		virtual void welcome(const Packet& p);
		virtual void announce(const Packet& p);
		virtual void gate_update(const Packet& p);
		//virtual void info(const Packet& p); // FIXME this packet is asymmetrical
		virtual void leave(const Packet& p);
		//virtual void player_animation(const Packet& p); // TODO revamp this packet
		virtual void request_denied(const Packet& p);
		virtual void name_change(const Packet& p);
		virtual void team_change(const Packet& p);
		//virtual void register_server(const Packet& p);
		//virtual void unregiser_server(const Packet& p);
		//virtual void upgrade_available(const Packet& p);
		//virtual void map_info(const Packet& p);
		//virtual void map_object(const Packet& p);
		//virtual void game_param(const Packet& p);
		//virtual void player_died(const Packet& p);
		virtual void weapon_info(const Packet& p);
		virtual void round_start(const Packet& p);
		virtual void spawn(const Packet& p);
		// End packet callbacks

		virtual void name_change(Player* player, const std::string& new_name);
		virtual void team_change(Player* player, char new_team);

		// Main loop: override for subclass behaviors, but call step inside
		virtual void run();
		
		// Called when client should disconnect from the server.
		virtual void disconnect();
	};
}

#endif
