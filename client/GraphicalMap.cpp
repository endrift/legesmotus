/*
 * client/GraphicalMap.cpp
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

#include "GraphicalMap.hpp"
#include "GameWindow.hpp"
#include "MapObjectParams.hpp"
#include "Sprite.hpp"
#include "BaseMapObject.hpp"
#include "TiledGraphic.hpp"
#include "common/PathManager.hpp"
#include <cstring>

using namespace LM;
using namespace std;

GraphicalMap::GraphicalMap(PathManager& path_manager, GameWindow* window) : m_path_manager(path_manager) {
	m_window = window;
}

void	GraphicalMap::clear() {
	m_cached_graphics.clear();
	ClientMap::clear();
}

bool	GraphicalMap::load(istream& in) {
	// Clear the graphics cache before and after loading the map.
	// This is to avoid possible memory management issues where a map object is removed after loading, but a dangling pointer to the graphic is left in m_cached_graphics.
	// Graphics caching is really only needed during initial map loading to make a significant difference anyways.
	m_cached_graphics.clear();
	bool	result = Map::load(in);
	m_cached_graphics.clear();
	return result;
}

template<class T> T* GraphicalMap::new_graphic(const std::string& sprite_name) {
	Graphic*&	graphic = m_cached_graphics[sprite_name];

	if (!graphic) {
		if (sprite_name.find_first_of("/\\") != string::npos) {
			// TODO: do this more nicely
			return new T(static_cast<SDL_Surface*>(NULL));
		}

		// Graphic not cached yet.
		string	filename(sprite_name);
		filename += ".png";

		T*	new_graphic = new T(m_path_manager.data_path(filename.c_str(), "sprites"));
		graphic = new_graphic;
		return new_graphic;
	}

	// Create this new graphic from the cached graphic.
	return new T(*graphic);
}

Graphic*	GraphicalMap::load_graphic(const std::string& graphic_name, bool is_centered, Point position, const MapObjectParams& graphic_params) {
	if (graphic_name == "-") {
		return NULL;
	}

	Graphic*		graphic = NULL;
	if (graphic_params.is_tiled) {
		TiledGraphic*	tiled_graphic = new_graphic<TiledGraphic>(graphic_name);
		tiled_graphic->set_width(graphic_params.tile_dimensions.x);
		tiled_graphic->set_height(graphic_params.tile_dimensions.y);
		graphic = tiled_graphic;
	} else {
		Sprite*		sprite = new_graphic<Sprite>(graphic_name);
		if (!is_centered) {
			// Make the sprite draw from the upper left, not the center
			sprite->set_center_x(0.0);
			sprite->set_center_y(0.0);
		}
		sprite->set_antialiasing(graphic_params.is_antialiased);
		graphic = sprite;
	}

	update_graphic(graphic, position, graphic_params);

	m_window->register_graphic(graphic);

	return graphic;
}

void	GraphicalMap::update_graphic(Graphic* graphic, Point position, const MapObjectParams& graphic_params) {
	if (graphic) {
		graphic->set_x(position.x);
		graphic->set_y(position.y);

		graphic->set_priority(graphic_params.priority);
		graphic->set_scale_x(graphic_params.scale_x);
		graphic->set_scale_y(graphic_params.scale_y);
		graphic->set_rotation(graphic_params.rotation);
	}
}

void	GraphicalMap::unregister_graphic(Graphic* graphic) {
	if (graphic) {
		m_window->unregister_graphic(graphic);
		delete graphic;
	}
}

void	GraphicalMap::set_visible(bool visible) {
	const list<BaseMapObject*>&	objects(get_objects());
	for (list<BaseMapObject*>::const_iterator it(objects.begin()); it != objects.end(); ++it) {
		if ((*it)->has_graphic()) {
			(*it)->get_graphic()->set_invisible(!visible);
		}
	}
}

