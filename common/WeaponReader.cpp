/*
 * common/WeaponReader.cpp
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

#include "WeaponReader.hpp"
#include "PacketReader.hpp"
#include "PacketWriter.hpp"
#include <string>
#include <string.h>
#include <algorithm>

using namespace LM;
using namespace std;

WeaponReader::WeaponReader() {
}

WeaponReader::WeaponReader(const char* weapon_data) : StringTokenizer(weapon_data, "\t", true) {
	if (const char* object_header = get_next()) {
		StringTokenizer(object_header, ':') >> m_id >> m_type;
	}
}

void	WeaponReader::init(const char* data) {
	StringTokenizer::init(data, "\t", true);
	if (const char* object_header = get_next()) {
		StringTokenizer(object_header, ':') >> m_id >> m_type;
	}
}

void	WeaponReader::swap(WeaponReader& other) {
	StringTokenizer::swap(other);
	// std:: prefix necessary here to avoid name conflicts
	std::swap(m_type, other.m_type);
	std::swap(m_id, other.m_id);
}

PacketReader&	LM::operator>>(PacketReader& packet, WeaponReader& weapon_reader) {
	packet >> weapon_reader.m_type >> weapon_reader.m_id;
	if (const char* data = packet.get_next()) {
		weapon_reader.StringTokenizer::init(data, "\t", true);
	} else {
		weapon_reader.StringTokenizer::init("", "\t", true);
	}
	return packet;
}

PacketWriter&	LM::operator<<(PacketWriter& packet, const WeaponReader& weapon_reader) {
	packet << weapon_reader.m_type << weapon_reader.m_id << weapon_reader.get_rest();
	return packet;
}

