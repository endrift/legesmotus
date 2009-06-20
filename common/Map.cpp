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
#include "misc.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <string.h>

using namespace LM;
using namespace std;

// See .hpp file for comments.

Map::Map() {
	m_width = m_height = 0;
}

Map::~Map() {
	Map::clear();
}

bool	Map::load(istream& in) {
	clear();

	// read the first three lines of the map file
	in >> m_name >> m_width >> m_height;

	// read each line one-by-one
	string			line;
	while (getline(in, line)) {
		// Strip any leading or trailing white space
		strip_leading_trailing_spaces(line);

		// Ignore blank lines and lines starting with # (for comments)
		// Not ignoring blank lines does cause problems, even if the map file doesn't appear to have blank lines in it.
		if (!line.empty() && line[0] != '#') {
			condense_whitespace(line, istab, '\t');

			MapReader	reader(line.c_str());
			add_object(reader);
		}
	}

	return true;
}

bool	Map::load_file(const char* path) {
	ifstream	file(path);
	return file && load(file);
}

void	Map::clear() {
	m_name.clear();
	m_width = m_height = 0;
}

Map::ObjectType	Map::parse_object_type(const char* type_string) {
	if (strcasecmp(type_string, "SPRITE") == 0)
		return SPRITE;
	if (strcasecmp(type_string, "GATE") == 0)
		return GATE;
	if (strcasecmp(type_string, "SPAWN") == 0)
		return SPAWN_POINT;
	
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

