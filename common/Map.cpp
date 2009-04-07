/*
 * common/Map.cpp
 *
 * Copyright 2009 - Nathan Partlan, Andrew Ayer, Daniel Schneider, and Jeffrey Pfau
 * 
 */

#include "Map.hpp"
#include "PacketReader.hpp"
#include <iostream>
#include <string>

using namespace std;

Map::Map() {
	m_width = m_height = 0;
}

void	Map::load_file(istream& in) {
	in >> m_name >> m_width >> m_height;

	string			line;
	while (getline(in, line)) {
		PacketReader	reader(line.c_str(), '~');
		add_object(reader);
	}
}

