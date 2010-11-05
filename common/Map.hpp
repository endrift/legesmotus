/*
 * common/Map.hpp
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

#ifndef LM_COMMON_MAP_HPP
#define LM_COMMON_MAP_HPP

#include <string>
#include <iosfwd>
#include <list>
#include "ConfigManager.hpp"

class b2World;

namespace LM {
	class MapReader;
	class StringTokenizer;
	class PacketReader;
	class PacketWriter;
	class MapObject;
	class ClientMapObject;
	
	/*
	 * A Map keeps track of things like the map name, dimensions, and spawn points
	 * The client should derive a class called GraphicalMap which handles graphics and stuff
	 * The server should derive a class called ServerMap which handles spawning
	 */
	class Map {
	public:
		enum ObjectType {
			INVALID_OBJECT_TYPE = 0,
			GATE = 2,
			SPAWN_POINT = 3,
			OBSTACLE = 4,
			DECORATION = 5,
			REPULSION = 6,
			FORCE_FIELD = 7,
			HAZARD = 8
		};
		static ObjectType	parse_object_type(const char* type_string);
	

	private:
		std::list<MapObject*> m_objects;

	protected:
		std::string	m_name;		// Should be unique
		int		m_revision;
		int		m_width;
		int		m_height;
		ConfigManager	m_options;

		virtual ClientMapObject* make_client_map_object(MapReader* reader) { return NULL; }

	public:
		Map();
		virtual ~Map();

		MapObject* make_map_object(MapReader* reader);

		// These really shouldn't be necessary (ticket #216)
		void set_width(int width) { m_width = width; }
		void set_height(int height) { m_height = height; }
		void set_revision(int revision) { m_revision = revision; }

		// Standard getters
		const char*	get_name() const { return m_name.c_str(); }
		int		get_revision() const { return m_revision; }
		int		get_width() const { return m_width; }
		int		get_height() const { return m_height; }
		const ConfigManager&	get_options() const { return m_options; }
		const std::list<MapObject*>& get_objects() const { return m_objects; }

		bool		is_loaded(const char* name, int revision) const;
		
		// Set up the physics for the map in a given world.
		virtual void	initialize_physics(b2World* world);
		
		// Read and parse the given input stream and load into the current map
		virtual bool	load(std::istream& in);
	
		// load_file will preserve the current map if it can't open the new map
		// (and return false to indicate error)
		virtual bool	load_file(const char* path);
	
		// Remove all objects from the map:
		virtual void	clear();

		// Parse the given packet representation of a map object and add it to the map
		// TODO remove this top one
		virtual void	add_object(MapReader& data);
		virtual void	add_object(MapObject* object);

		friend PacketReader&	operator>>(PacketReader& packet, Map& map);
		friend PacketWriter&	operator<<(PacketWriter& packet, const Map& map);
	};
	
	StringTokenizer&	operator>> (StringTokenizer&, Map::ObjectType&);
	
	
}

#endif
