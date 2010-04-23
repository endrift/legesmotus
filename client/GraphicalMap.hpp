/*
 * client/GraphicalMap.hpp
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

#ifndef LM_CLIENT_GRAPHICALMAP_HPP
#define LM_CLIENT_GRAPHICALMAP_HPP

#include "client/ClientMap.hpp"
#include <map>

namespace LM {
	class GameWindow;
	class PathManager;
	
	/*
	 * Derives from ClientMap, and implements the graphics stuff.
	 */
	class GraphicalMap : public ClientMap {
	private:
		PathManager&			m_path_manager;
		GameWindow* 			m_window;
		std::map<std::string, Graphic*>	m_cached_graphics;
	
		template<class T> T*		new_graphic(const std::string& sprite_name);

	public:
		GraphicalMap(PathManager&, GameWindow* window);
	
		virtual void			clear(); // Remove all objects

		// Read and parse the given input stream and load into the current map
		bool				load(std::istream& in);
	
		void				set_visible(bool visible);

		virtual Graphic*		load_graphic(const std::string& graphic_name, bool is_centered,  Point position, const MapObjectParams& graphic_params);
		virtual void			update_graphic(Graphic* g, Point position, const MapObjectParams& graphic_params);
		virtual void			unregister_graphic(Graphic* g);
	};
}

#endif
