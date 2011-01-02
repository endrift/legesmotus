/*
 * gui/GraphicalMap.cpp
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

#include "GraphicalMap.hpp"
#include "GraphicalMapObject.hpp"
#include "GraphicalGate.hpp"
#include "ResourceCache.hpp"
#include "GraphicRegion.hpp"
#include "common/MapObject.hpp"
#include "common/MapReader.hpp"

using namespace LM;
using namespace std;

GraphicalMap::GraphicalMap(ResourceCache *cache) {
	m_cache = cache;
}

GraphicalMap::~GraphicalMap() {
}

GraphicalMapObject* GraphicalMap::make_client_map_object(MapReader* reader) {
	switch(reader->get_type()) {
	case Map::GATE:
		return new GraphicalGate(m_cache);
	default:
		return new GraphicalMapObject(m_cache);
	}
}

void GraphicalMap::add_object(MapObject* object) {
	Map::add_object(object);
	GraphicalMapObject* obj = static_cast<GraphicalMapObject*>(object->get_client_part());
	// TODO What if it's foreground!?
	if (!(obj->get_graphic() == NULL)) {
		m_background.add_graphic(obj->get_graphic());
	}
}

GraphicContainer* GraphicalMap::get_background() {
	return &m_background;
}
