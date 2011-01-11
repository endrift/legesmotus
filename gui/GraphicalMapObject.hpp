/*
 * gui/GraphicalMapObject.hpp
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

#ifndef LM_GUI_GRAPHICALMAPOBJECT_HPP
#define LM_GUI_GRAPHICALMAPOBJECT_HPP

#include "common/ClientMapObject.hpp"
#include "Graphic.hpp"
#include <string>

namespace LM {
	class GraphicRegion;
	class ResourceCache;

	class GraphicalMapObject : public ClientMapObject {
	private:
		GraphicRegion* m_graphic;
		ResourceCache* m_cache;

	protected:
		void load_graphic(const std::string& imagename);

	public:
		GraphicalMapObject(ResourceCache* cache);
		~GraphicalMapObject();

		virtual void read(MapReader* reader, MapObject* owner);

		virtual void set_position(Point position); 
		virtual void set_is_tiled(bool is_tiled);
		virtual void set_tile_dimensions(Vector tile_dimensions);
		virtual void set_scale_x(float scale_x);
		virtual void set_scale_y(float scale_y);
		virtual void set_rotation(float rotation);

		GraphicRegion* get_graphic();
	};
}

#endif
