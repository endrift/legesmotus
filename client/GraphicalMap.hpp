/*
 * client/GraphicalMap.hpp
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

#ifndef LM_CLIENT_GRAPHICALMAP_HPP
#define LM_CLIENT_GRAPHICALMAP_HPP

#include "common/Map.hpp"
#include "client/MapObject.hpp"
#include <list>
#include <map>

namespace LM {
	class GameWindow;
	class PathManager;
	class Graphic;
	
	/*
	 * Derives from Map, and adds graphics stuff.
	 */
	class GraphicalMap : public Map {
	private:
		PathManager&			m_path_manager;
		GameWindow* 			m_window;
		std::list<MapObject>		m_objects;
		MapObject*			m_gates[2];
		std::map<std::string, Graphic*>	m_cached_graphics;
	
		enum {
			GATE_HEIGHT = 109,	// Height of the gate bar
			GATE_EXTENT = 24	// Extent to which the gate's bounding polygon should extend outwards horizontally from the actual sprite
		}; // XXX: Don't hard code
	
		MapObject*			get_gate_object(char team);
	
		template<class T> T*		load_graphic(const std::string& sprite_name);
	
	public:
		GraphicalMap(PathManager&, GameWindow* window);
		virtual ~GraphicalMap();
	
		const std::list<MapObject>&	get_objects() const { return m_objects; }
		size_t				nbr_objects() const { return m_objects.size(); }
		virtual void			clear(); // Remove all objects
	
		// Read and parse the given input stream and load into the current map
		bool				load(std::istream& in);
	
		virtual void			set_visible(bool visible);
		virtual void			add_object(MapReader& data);
	
		// progress is in [0.0,1.0], where 0 == closed .. 1 == fully open
		void				set_gate_progress(char team, double progress);
		void				reset_gates();

		void				reset(); // Reset the map for a new round
	};
}

#endif
