/*
 * common/Map.cpp
 *
 * Copyright 2009 - Nathan Partlan, Andrew Ayer, Daniel Schneider, and Jeffrey Pfau
 * 
 */

#include "Map.hpp"
#include "PacketReader.hpp"
#include "misc.hpp"
#include <iostream>
#include <fstream>
#include <string>

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
		// Strip any trailing white space
		strip_trailing_spaces(line);

		// Ignore blank lines and lines starting with # (for comments)
		// Not ignoring blank lines does cause problems, even if the map file doesn't appear to have blank lines in it.
		if (!line.empty() && line[0] != '#') {
			PacketReader	reader(line.c_str(), '~');
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
