/*
 * common/MapReader.cpp
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

#include "MapReader.hpp"
#include "PacketReader.hpp"
#include "PacketWriter.hpp"
#include <string>
#include <string.h>
#include <algorithm>

using namespace LM;
using namespace std;

MapReader::MapReader() {
	m_type = Map::INVALID_OBJECT_TYPE;
}

MapReader::MapReader(const char* map_object_data) : StringTokenizer(map_object_data, "\t", true) {
	if (const char* object_header = get_next()) {
		if (strchr(object_header, ':')) {
			StringTokenizer(object_header, ':') >> m_id >> m_type;
		} else {
			m_type = Map::parse_object_type(object_header);
		}
	}
}

void	MapReader::swap(MapReader& other) {
	StringTokenizer::swap(other);
	// std:: prefix necessary here to avoid name conflicts
	std::swap(m_type, other.m_type);
	std::swap(m_id, other.m_id);
}

PacketReader&	LM::operator>>(PacketReader& packet, MapReader& map_object) {
	int	type_int;
	packet >> type_int >> map_object.m_id;
	map_object.m_type = Map::ObjectType(type_int);
	if (const char* data = packet.get_next()) {
		map_object.StringTokenizer::init(data, "\t", true);
	} else {
		map_object.StringTokenizer::init("", "\t", true);
	}
	return packet;
}

PacketWriter&	LM::operator<<(PacketWriter& packet, const MapReader& map_object) {
	packet << int(map_object.m_type) << map_object.m_id << map_object.get_rest();
	return packet;
}

