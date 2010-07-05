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

#include <sstream>

using namespace LM;
using namespace std;

void ServerList::json_begin(ostream *outfile, const std::string& root) {
	json_begin_dict(outfile);
	json_add_dict_entry(outfile, root);
	json_begin_dict(outfile);
}

void ServerList::json_end(ostream *outfile) {
	json_end_dict(outfile);
	json_end_dict(outfile);
}
void ServerList::json_begin_dict(ostream *outfile) {
	if (m_json_needs_comma) {
		(*outfile) << ",";
		json_indent(outfile);
	}

	(*outfile) << "{";
	++m_json_indentation;
	json_indent(outfile);
	m_json_needs_comma = false;
}

void ServerList::json_add_dict_entry(ostream *outfile, const std::string& name) {
	if (m_json_needs_comma) {
		(*outfile) << ",";
		json_indent(outfile);
	}

	m_json_needs_comma = false;
	json_add_string(outfile, name);
	m_json_needs_comma = false;
	(*outfile) << ": ";
}

void ServerList::json_end_dict(ostream *outfile) {
	--m_json_indentation;
	json_indent(outfile);
	(*outfile) << "}";
	m_json_needs_comma = true;
}
void ServerList::json_begin_list(ostream *outfile) {
	(*outfile) << "[";
	++m_json_indentation;
	json_indent(outfile);
	m_json_needs_comma = false;
}

void ServerList::json_end_list(ostream *outfile) {
	--m_json_indentation;
	json_indent(outfile);
	(*outfile) << "]";
	m_json_needs_comma = true;
}

void ServerList::json_add_string(ostream *outfile, const std::string& str) {
	if (m_json_needs_comma) {
		(*outfile) << ",";
		json_indent(outfile);
	}

	string escaped = str;
	// TODO escape;
	escaped = '"' + escaped + '"';

	(*outfile) << escaped;
	m_json_needs_comma = true;
}

void ServerList::json_add_int(ostream *outfile, int num) {
	if (m_json_needs_comma) {
		(*outfile) << ",";
		json_indent(outfile);
	}

	(*outfile) << num;
	m_json_needs_comma = true;
}

void ServerList::json_add_int(ostream *outfile, uint64_t num) {
	if (m_json_needs_comma) {
		(*outfile) << ",";
		json_indent(outfile);
	}

	(*outfile) << '"' <<  num << '"';
	m_json_needs_comma = true;
}

void ServerList::json_indent(ostream *outfile) {
	(*outfile) << '\n';
	for (int i = 0; i < m_json_indentation; ++i) {
		(*outfile) << '\t';	
	}
}

void ServerList::add(const IPAddress& ipaddr, const Server& server) {
	m_list[ipaddr] = server;
}

void ServerList::output(ostream *outfile) {
	stringstream buffer;

	json_begin(outfile, "scan");
	json_add_dict_entry(outfile, "servers");
	json_begin_list(outfile);
	for (map<IPAddress, Server>::const_iterator iter = m_list.begin(); iter != m_list.end(); ++iter) {
		json_begin_dict(outfile);

		json_add_dict_entry(outfile, "ip_address");
		buffer << iter->first << flush;
		json_add_string(outfile, buffer.str());
		buffer.str(""); // TODO IPAddress std::String cast

		json_add_dict_entry(outfile, "map_name");
		json_add_string(outfile, iter->second.current_map_name);

		json_add_dict_entry(outfile, "team_count");

		json_begin_list(outfile);
		json_add_int(outfile, iter->second.team_count[0]);
		json_add_int(outfile, iter->second.team_count[1]);
		json_end_list(outfile);

		json_add_dict_entry(outfile, "max_players");
		json_add_int(outfile, iter->second.max_players);

		json_add_dict_entry(outfile, "uptime");
		json_add_int(outfile, iter->second.uptime);

		json_add_dict_entry(outfile, "time_left_in_game");
		json_add_int(outfile, iter->second.time_left_in_game);

		json_add_dict_entry(outfile, "server_name");
		json_add_string(outfile, iter->second.server_name);

		json_add_dict_entry(outfile, "server_location");
		json_add_string(outfile, iter->second.server_location);

		json_add_dict_entry(outfile, "ping");
		json_add_int(outfile, iter->second.ping);

		json_end_dict(outfile);
	}
	json_end_list(outfile);
	json_end(outfile);
}
