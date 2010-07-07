/*
 * serverscanner/ServerList.cpp
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

#include "ServerList.hpp"
#include "common/timer.hpp"

#include <sstream>

using namespace LM;
using namespace std;

void ServerList::add(const IPAddress& ipaddr, const Server& server) {
	m_list[ipaddr] = server;
}

void ServerList::output(OutputGenerator *out) {
	stringstream buffer;

	out->begin();
	out->add_dict_entry("servers");
	out->begin_list();
	for (map<IPAddress, Server>::const_iterator iter = m_list.begin(); iter != m_list.end(); ++iter) {
		out->begin_dict();

		out->add_dict_entry("ip_address");
		buffer << iter->first << flush;
		out->add_string(buffer.str());
		buffer.str(""); // TODO IPAddress std::String cast

		out->add_dict_entry("map_name");
		out->add_string(iter->second.current_map_name);

		out->add_dict_entry("team_count");

		out->begin_list();
		out->add_int(iter->second.team_count[0]);
		out->add_int(iter->second.team_count[1]);
		out->end_list();

		out->add_dict_entry("max_players");
		out->add_int(iter->second.max_players);
		out->add_dict_entry("uptime");
		out->add_int(iter->second.uptime);

		out->add_dict_entry("time_left_in_game");
		out->add_int(iter->second.time_left_in_game);

		out->add_dict_entry("server_name");
		out->add_string(iter->second.server_name);

		out->add_dict_entry("server_location");
		out->add_string(iter->second.server_location);

		out->add_dict_entry("ping");
		out->add_int(iter->second.ping);

		out->end_dict();
	}
	out->end_list();

	out->add_dict_entry("timestamp");
	out->add_int(utc_time());

	out->end();
}
