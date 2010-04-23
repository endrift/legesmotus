/*
 * client/Decoration.cpp
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

#include "Decoration.hpp"
#include <string>
#include "common/MapReader.hpp"
#include "GameController.hpp"
#include "ClientMap.hpp"

using namespace LM;
using namespace std;

Decoration::Decoration (Point position) : BaseMapObject(position) {
	m_graphic = NULL;
	m_params.priority = 512; // TODO: use enum
}

void	Decoration::init (MapReader& reader, ClientMap& map) {
	reader >> m_graphic_name;

	while (reader.has_more()) {
		string		param_string;
		reader >> param_string;
		m_params.parse(param_string.c_str());
	}

	if (!m_graphic) {
		m_graphic = map.load_graphic(m_graphic_name, false, get_position(), m_params);
	}
}

