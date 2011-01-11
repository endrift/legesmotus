/*
 * gui/GraphicalGate.cpp
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

#include "GraphicalGate.hpp"
#include "GraphicRegion.hpp"
#include "common/Gate.hpp"

using namespace LM;
using namespace std;

GraphicalGate::GraphicalGate(ResourceCache* cache) : GraphicalMapObject(cache) {
	// Nothing to do
}

void GraphicalGate::read(MapReader* reader, MapObject* owner) {
	const char* team_name;
	Gate* gate = static_cast<Gate*>(owner);
	switch (gate->get_team()) {
	case 'A':
		team_name = "blue";
		break;
	case 'B':
		team_name = "red";
		break;
	default:
		return;
	}
	load_graphic(team_name + string("_gate"));
	get_graphic()->set_width(gate->get_width());
	get_graphic()->set_height(gate->get_length());
	m_length = gate->get_length();
}

void GraphicalGate::set_position(Point position) {
	get_graphic()->set_x(position.x + 27);
	get_graphic()->set_y(position.y);
}

void GraphicalGate::set_scale_y(float scale_y) {
	get_graphic()->set_height(m_length*scale_y);
}
