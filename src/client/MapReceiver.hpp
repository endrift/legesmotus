/*
 * client/MapReceiver.hpp
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

#ifndef LM_CLIENT_MAPRECEIVER_HPP
#define LM_CLIENT_MAPRECEIVER_HPP

#include <stddef.h>
#include <stdint.h>
#include <set>

namespace LM {
	class Map;
	class PacketReader;

	class MapReceiver {
	private:
		static uint32_t		next_transmission_id;

		Map&			m_map;
		uint32_t		m_transmission_id;
		bool			m_has_info;
		size_t			m_nbr_expected_objects;
		size_t			m_nbr_received_objects;

	public:
		explicit MapReceiver(Map& map);

		uint32_t		transmission_id () const { return m_transmission_id; }

		// This function returns true when the entire map has been received
		// Returns false if there is still more data to be received
		bool			is_done() const { return m_has_info && m_nbr_received_objects >= m_nbr_expected_objects; }

		// These functions return true if the packet was accepted,
		// or false if it was rejected (e.g. because it's not ready for the packet)
		bool			map_info(PacketReader& map_info_packet);
		bool			map_object(PacketReader& map_object_packet);
	};
}

#endif
