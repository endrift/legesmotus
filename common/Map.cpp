/*
 * common/Map.cpp
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

#include "Map.hpp"
#include "MapReader.hpp"
#include "StringTokenizer.hpp"
#include "PacketReader.hpp"
#include "PacketWriter.hpp"
#include "misc.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <string.h>

using namespace LM;
using namespace std;

// See .hpp file for comments.

Map::Map() {
	m_revision = 0;
	m_width = m_height = 0;
}

Map::~Map() {
	Map::clear();
}

bool	Map::is_loaded(const char* name, int revision) const {
	return !m_name.empty() && m_name == name && m_revision == revision;
}

bool	Map::load(istream& in) {
	clear();

	string			line;

	/*
	 * Map header: specifies map options (e.g. name, width, etc.)
	 * This section ends when a blank line is encountered.
	 * No blank lines are allowed in this section, but comments are allowed.
	 */
	while (getline(in, line)) {
		if (line[0] == ';' || line[0] == '#') {
			// This line is a comment
			continue;
		}

		StringTokenizer	tokenizer(line, " \t", true, 2);
		string		option_name;
		tokenizer >> option_name;

		if (option_name.empty()) {
			// Blank line -> end of map header
			break;
		}

		if (option_name == "name") {
			tokenizer >> m_name;
		} else if (option_name == "revision") {
			tokenizer >> m_revision;
		} else if (option_name == "width") {
			tokenizer >> m_width;
		} else if (option_name == "height") {
			tokenizer >> m_height;
		} else {
			// Miscellaneous map option (e.g. game mode, max_players, etc.)
			// Ultimately used for initializing the GameParameters object.
			tokenizer >> m_options[option_name.c_str()];
		}
	}

	/*
	 * Map body: specifies map objects
	 * This section continues for the rest of the file.
	 */
	while (getline(in, line)) {
		// Strip any leading or trailing white space
		strip_leading_trailing_spaces(line);

		// Ignore blank lines and lines starting with # or ; (for comments)
		if (line.empty() || line[0] == '#' || line[0] == ';') {
			continue;
		}

		MapReader	reader(line.c_str());
		add_object(reader);
	}

	return true;
}

bool	Map::load_file(const char* path) {
	ifstream	file(path);
	return file && load(file);
}

void	Map::clear() {
	m_name.clear();
	m_revision = 0;
	m_width = m_height = 0;
	m_options.clear();
}

Map::ObjectType	Map::parse_object_type(const char* type_string) {
	if (strcasecmp(type_string, "GATE") == 0)
		return GATE;
	if (strcasecmp(type_string, "SPAWN") == 0)
		return SPAWN_POINT;
	if (strcasecmp(type_string, "OBSTACLE") == 0)
		return OBSTACLE;
	if (strcasecmp(type_string, "DECORATION") == 0 || strcasecmp(type_string, "BACKGROUND") == 0)
		return DECORATION;
	if (strcasecmp(type_string, "REPULSION") == 0)
		return REPULSION;
	if (strcasecmp(type_string, "FORCE") == 0)
		return FORCE_FIELD;
	if (strcasecmp(type_string, "HAZARD") == 0)
		return HAZARD;
	
	return INVALID_OBJECT_TYPE;
}

StringTokenizer&	LM::operator>> (StringTokenizer& tok, Map::ObjectType& object_type) {
	if (const char* str = tok.get_next()) {
		object_type = Map::parse_object_type(str);
	} else {
		object_type = Map::INVALID_OBJECT_TYPE;
	}
	return tok;
}

PacketReader&	LM::operator>>(PacketReader& packet, Map& map) {
	packet >> map.m_name >> map.m_revision >> map.m_width >> map.m_height;
	return packet;
}

PacketWriter&	LM::operator<<(PacketWriter& packet, const Map& map) {
	packet << map.m_name << map.m_revision << map.m_width << map.m_height;
	return packet;
}

