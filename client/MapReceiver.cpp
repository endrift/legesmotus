/*
 * client/MapReceiver.cpp
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

#include "MapReceiver.hpp"
#include "common/Map.hpp"
#include "common/MapReader.hpp"
#include "common/misc.hpp"
#include "common/PacketReader.hpp"

using namespace LM;
using namespace std;

uint32_t MapReceiver::next_transmission_id = 1L;

MapReceiver::MapReceiver(Map& map) : m_map(map) {
	m_transmission_id = next_transmission_id++;
	m_has_info = false;
	m_nbr_expected_objects = 0;
	m_nbr_received_objects = 0;
}

bool	MapReceiver::map_info(PacketReader& packet) {
	uint32_t	transmission_id;
	packet >> transmission_id;

	if (transmission_id != m_transmission_id || m_has_info) {
		// Packet rejected, because:
		//  wrong transmission ID (probably from an old transmission), or
		//  info already received
		return false;
	}
	m_has_info = true;
	packet >> m_map >> m_nbr_expected_objects;
	return true;
}

bool	MapReceiver::map_object(PacketReader& packet) {
	uint32_t	transmission_id;
	packet >> transmission_id;

	if (transmission_id != m_transmission_id || !m_has_info || is_done()) {
		// Packet rejected, beacuse:
		//  wrong transmission ID (probably from an old transmission), or
		//  map info not received yet, or
		//  map already completely received
		return false;
	}

	MapReader		map_object;
	packet >> map_object;
	m_map.add_object(map_object);

	++m_nbr_received_objects;
	return true;
}

