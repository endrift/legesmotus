/*
 * gui/GraphicalMapObject.cpp
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

#include "GraphicalMapObject.hpp"
#include "Image.hpp"
#include "common/MapReader.hpp"
#include "common/MapObject.hpp"
#include "GraphicRegion.hpp"
#include "ResourceCache.hpp"

using namespace LM;
using namespace std;

GraphicalMapObject::GraphicalMapObject(ResourceCache* cache) {
	m_cache = cache;
	m_graphic = NULL;
}

GraphicalMapObject::~GraphicalMapObject() {
	delete m_graphic;
}

void GraphicalMapObject::load_graphic(const string& imagename) {
	Image image(imagename + ".png", m_cache, true);
	delete m_graphic;
	m_graphic = new GraphicRegion(&image);
	m_graphic->set_width(image.get_width());
	m_graphic->set_height(image.get_height());
}

void GraphicalMapObject::read(MapReader* reader, MapObject* owner) {
	string graphic_name;
	(*reader) >> graphic_name;

	if (graphic_name == "-") {
		return;
	}

	load_graphic(graphic_name);
	m_graphic->set_x(owner->get_position().x);
	m_graphic->set_y(owner->get_position().y);

	// TODO other properties
}

void GraphicalMapObject::set_position(Point position) {
	m_graphic->set_x(position.x);
	m_graphic->set_y(position.y);
}

void GraphicalMapObject::set_is_tiled(bool is_tiled) {
	m_graphic->set_image_repeat(is_tiled);
}

void GraphicalMapObject::set_tile_dimensions(Vector tile_dimensions) {
	m_graphic->set_width(tile_dimensions.x);
	m_graphic->set_height(tile_dimensions.y);
}

void GraphicalMapObject::set_scale_x(float scale_x) {
	m_graphic->set_scale_x(scale_x);
}

void GraphicalMapObject::set_scale_y(float scale_y) {
	m_graphic->set_scale_y(scale_y);
}

void GraphicalMapObject::set_rotation(float rotation) {
	m_graphic->set_rotation(rotation);
}

GraphicRegion* GraphicalMapObject::get_graphic() {
	return m_graphic;
}
