/*
 * serverscanner/ServerList.hpp
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

#ifndef LM_CLIENT_SERVERLIST_HPP
#define LM_CLIENT_SERVERLIST_HPP

#include "common/IPAddress.hpp"

#include <ostream>
#include <string>
#include <map>

namespace LM {
	class ServerList {
		public:
			struct Server {
				std::string	current_map_name;
				int		team_count[2];
				int		max_players;
				uint64_t	uptime;
				uint64_t	time_left_in_game;
				std::string	server_name;
				std::string	server_location;
				uint64_t	ping;
			};

		private:
			std::map<IPAddress, Server> m_list;

			// JSON stuff
			// TODO move to class
			int m_json_indentation;
			bool m_json_needs_comma;

			void json_begin(std::ostream *outfile);
			void json_end(std::ostream *outfile);

			void json_begin_dict(std::ostream *outfile);
			void json_add_dict_entry(std::ostream *outfile, const std::string& name);
			void json_end_dict(std::ostream *outfile);

			void json_begin_list(std::ostream *outfile);
			void json_end_list(std::ostream *outfile);

			void json_add_string(std::ostream *outfile, const std::string& str);
			void json_add_int(std::ostream *outfile, int num);
			void json_add_int(std::ostream *outfile, uint64_t num);

			void json_indent(std::ostream *outfile);

		public:
			void add(const IPAddress& ipaddr, const Server& server);
			void output(std::ostream *outfile);
	};
}

#endif
