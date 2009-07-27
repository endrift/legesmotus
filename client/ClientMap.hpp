/*
 * client/ClientMap.hpp
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

#ifndef LM_CLIENT_CLIENTMAP_HPP
#define LM_CLIENT_CLIENTMAP_HPP

#include "common/Map.hpp"
#include "common/Point.hpp"
#include <list>
#include <string>

namespace LM {
	class ClientGate;
	class BaseMapObject;
	class MapObjectParams;
	class Graphic;
	class Shape;

	/*
	 * Derives from Map, and adds client stuff (e.g. graphics)
	 * By default this class does not handle graphics.
	 * You must derive another class to do graphics.
	 */
	class ClientMap : public Map {
	private:
		std::list<BaseMapObject*>	m_objects;
		ClientGate*			m_gates[2];	// [0] == A (blue), [1] == B (red)

		ClientGate*			get_gate_object(char team);
	public:
		ClientMap ();
		virtual ~ClientMap ();

		const std::list<BaseMapObject*>& get_objects() const { return m_objects; }
		size_t				nbr_objects() const { return m_objects.size(); }
		virtual void			clear(); // Remove all objects

		virtual void			add_object(MapReader& data);

		// In this class, do nothing about graphics
		virtual Graphic*		load_graphic(const std::string& graphic_name, bool is_centered,  Point position, const MapObjectParams& graphic_params) { return NULL; }
		virtual void			update_graphic(Graphic* g, Point position, const MapObjectParams& graphic_params) { }
		virtual void			unregister_graphic(Graphic* g) { }
	
		void				register_gate(char team, ClientGate* gate);
		// progress is in [0.0,1.0], where 0 == closed .. 1 == fully open
		void				set_gate_progress(char team, double progress);
		void				reset_gates();

		void				reset(); // Reset the map for a new round

		static Shape*			make_bounding_shape(const std::string& shape_string, Point position, const MapObjectParams& params);
	};
}

#endif
