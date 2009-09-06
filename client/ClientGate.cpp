/*
 * client/ClientGate.cpp
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

#include "ClientGate.hpp"
#include "ClientMap.hpp"
#include "GameController.hpp"
#include "common/MapReader.hpp"
#include "common/Polygon.hpp"
#include "common/team.hpp"
#include <string>
#include <cstring>
#include <cstdlib>

using namespace LM;
using namespace std;

ClientGate::ClientGate(Point position) : BaseMapObject(position) {
	m_graphic = NULL;
	m_team = 0;
	m_width = 8;
	m_length = 109;
	m_extent = 24;
	m_progress = 0.0;
	m_params.scale_y = m_length;
	m_params.priority = 256; // TODO: use enum
	m_params.is_antialiased = false;
	m_is_engaged = false;
}

void	ClientGate::set_progress(ClientMap& map, double progress) {
	m_progress = progress;
	m_params.scale_y = m_length * (1.0 - progress);

	if (m_graphic) {
		map.update_graphic(m_graphic, get_position(), m_params);
	}

}

void	ClientGate::interact (GameController& gc, Player& player) {
	m_is_engaged = true;

	if (player.get_team() != m_team) {
		gc.set_gate_hold(!player.is_frozen());
	}
}

void	ClientGate::disengage (GameController& gc, Player& player) {
	m_is_engaged = false;

	gc.set_gate_hold(false);
}


void	ClientGate::init (MapReader& reader, ClientMap& map) {
	reader >> m_graphic_name;

	while (reader.has_more()) {
		string		param_string;
		reader >> param_string;
		if (strncmp(param_string.c_str(), "team=", 5) == 0) {
			m_team = parse_team_string(param_string.c_str() + 5);
		} else if (strncmp(param_string.c_str(), "width=", 6) == 0) {
			m_width = atof(param_string.c_str() + 6);
		} else if (strncmp(param_string.c_str(), "length=", 7) == 0) {
			m_params.scale_y = m_length = atof(param_string.c_str() + 7);
		} else if (strncmp(param_string.c_str(), "extent=", 7) == 0) {
			m_extent = atof(param_string.c_str() + 7);
		} else if (strncmp(param_string.c_str(), "rotate=", 7) == 0) {
			m_params.rotation = atof(param_string.c_str() + 7);
		} 
	}

	Polygon*	polygon = new Polygon(get_position() - Point(m_extent, 0));
	m_bounding_shape.reset(polygon);
	polygon->make_rectangle(m_width + m_extent * 2.0, m_length);
	polygon->rotate(m_params.rotation);

	if (!m_graphic) {
		m_graphic = map.load_graphic(m_graphic_name, false, get_position(), m_params);
	}

	map.register_gate(m_team, this);
}

