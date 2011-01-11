/*
 * serverscanner/ServerList.cpp
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

#include "ServerList.hpp"
#include "common/timer.hpp"

#include <sstream>

using namespace LM;
using namespace std;

void ServerList::add(const IPAddress& ipaddr, const Server& server) {
	m_list[ipaddr] = server;
}

void ServerList::output(OutputGenerator *out, uint64_t ticks) {
	stringstream buffer;

	out->add_column("ip_address", "Address");
	out->add_column("map_name", "Map name");
	out->add_column("team_count", "Players");
	out->add_column("max_players", "Max players");
	out->add_column("uptime", "Uptime");
	out->add_column("time_left_in_game", "Time left");
	out->add_column("server_location", "Location");
	out->add_column("server_name", "Name");
	out->add_column("ping", "Ping");
	out->add_column("timestamp", "Scan time");
	out->add_column("duration", "Scan duration");
	out->add_column("servers", "Servers");

	out->begin();
	out->begin_row();
	if (!m_list.empty()) {
		out->add_cell("servers");
		out->begin_list();
		for (map<IPAddress, Server>::const_iterator iter = m_list.begin(); iter != m_list.end(); ++iter) {
			out->begin_row();

			out->add_cell("ip_address");
			buffer << iter->first << flush;
			out->add_string(buffer.str());
			buffer.str(""); // TODO IPAddress std::String cast

			out->add_cell("map_name");
			out->add_string(iter->second.current_map_name);

			out->add_cell("team_count");

			out->begin_list();
			out->add_int(iter->second.team_count[0]);
			out->add_int(iter->second.team_count[1]);
			out->end_list();

			out->add_cell("max_players");
			out->add_int(iter->second.max_players);
			out->add_cell("uptime");
			out->add_interval(iter->second.uptime);

			out->add_cell("time_left_in_game");
			out->add_interval(iter->second.time_left_in_game);

			out->add_cell("server_name");
			out->add_string(iter->second.server_name);

			out->add_cell("server_location");
			out->add_string(iter->second.server_location);

			out->add_cell("ping");
			out->add_interval(iter->second.ping);

			out->end_row();
		}
		out->end_list();
	}

	out->add_cell("timestamp");
	out->add_time(utc_time());

	out->add_cell("duration");
	out->add_interval(ticks);

	out->end_row();
	out->end();
}
