/*
 * common/Map.cpp
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

#include "Map.hpp"
#include "MapReader.hpp"
#include "MapObject.hpp"
#include "ClientMapObject.hpp"
#include "StringTokenizer.hpp"
#include "PacketReader.hpp"
#include "PacketWriter.hpp"
#include "misc.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <string.h>
#include "physics.hpp"
#include "math.hpp"

using namespace LM;
using namespace std;

// See .hpp file for comments.

Map::Map() {
	m_revision = 0;
	m_width = m_height = 0;
	for (int i = 0; i < 4; i++) {
		m_edges[i] = new PhysicsObject;
	};
}

Map::~Map() {
	Map::clear();
	
	for (int i = 0; i < 4; i++) {
		delete m_edges[i];
	}
}

Gate* Map::get_gate(char team) {
	if (m_gates.find(team) == m_gates.end()) {
		return NULL;
	}
	
	return m_gates.find(team)->second;
}

const Gate* Map::get_gate(char team) const {
	if (m_gates.find(team) == m_gates.end()) {
		return NULL;
	}
	
	return m_gates.find(team)->second;
}

void Map::initialize_physics(b2World* world) {
	// Set up barriers around the map edges
	// The extents of box shapes are the half-widths of the box.
	
	// Bottom edge
	b2BodyDef bedgebodydef;
	bedgebodydef.position.Set(to_physics(get_width()/2.0f), to_physics(get_height() + EDGE_WIDTH)); // 
	b2Body* bedgebody = world->CreateBody(&bedgebodydef);
	bedgebody->SetUserData(m_edges[0]);
	b2PolygonShape bedgebox;
	bedgebox.SetAsBox(to_physics(get_width()/2 + 2 * EDGE_WIDTH), to_physics(EDGE_WIDTH));
	bedgebody->CreateFixture(&bedgebox, 0.0f);
	
	// Top edge
	b2BodyDef tedgebodydef;
	tedgebodydef.position.Set(to_physics(get_width()/2.0f), to_physics(-1 * EDGE_WIDTH)); // 
	b2Body* tedgebody = world->CreateBody(&tedgebodydef);
	tedgebody->SetUserData(m_edges[1]);
	b2PolygonShape tedgebox;
	tedgebox.SetAsBox(to_physics(get_width()/2 + 2 * EDGE_WIDTH), to_physics(EDGE_WIDTH));
	tedgebody->CreateFixture(&tedgebox, 0.0f);
	
	// Left edge
	b2BodyDef ledgebodydef;
	ledgebodydef.position.Set(to_physics(-1 * EDGE_WIDTH), to_physics(get_height()/2.0f)); // 
	b2Body* ledgebody = world->CreateBody(&ledgebodydef);
	ledgebody->SetUserData(m_edges[2]);
	b2PolygonShape ledgebox;
	ledgebox.SetAsBox(to_physics(EDGE_WIDTH), to_physics(get_height()/2 + 2 * EDGE_WIDTH));
	ledgebody->CreateFixture(&ledgebox, 0.0f);
	
	// Right edge
	b2BodyDef redgebodydef;
	redgebodydef.position.Set(to_physics(get_width() + EDGE_WIDTH), to_physics(get_height()/2.0f)); // 
	b2Body* redgebody = world->CreateBody(&redgebodydef);
	redgebody->SetUserData(m_edges[3]);
	b2PolygonShape redgebox;
	redgebox.SetAsBox(to_physics(EDGE_WIDTH), to_physics(get_height()/2 + 2 * EDGE_WIDTH));
	redgebody->CreateFixture(&redgebox, 0.0f);

	// Now, set up the physics for each map object.
	list<MapObject*>::iterator it;
	for (it = m_objects.begin(); it != m_objects.end(); it++) {
		(*it)->initialize_physics(world);
	}
}

MapObject* Map::make_map_object(MapReader* reader) {
	Point				position;
	(*reader) >> position;

	MapObject* object;
	ClientMapObject* clientpart = make_client_map_object(reader);

	switch (reader->get_type()) {
	case Map::OBSTACLE:
	case Map::HAZARD:
		object = new Obstacle(position, clientpart);
		break;
	case Map::DECORATION:
		object = new Decoration(position, clientpart);
		break;
	case Map::GATE:
		object = new Gate(position, clientpart);
		break;
	case Map::FORCE_FIELD:
	case Map::REPULSION:
		object = new ForceField(position, clientpart);
		break;
	default:
		delete clientpart;
		return NULL;
	}

	object->init(reader);
	
	// If we are adding a gate, add it to the gates map.
	if (reader->get_type() == Map::GATE) {
		m_gates.insert(pair<char, Gate*>(object->get_team(), static_cast<Gate*>(object)));
	}
	
	return object;
}

bool Map::is_loaded(const char* name, int revision) const {
	return !m_name.empty() && m_name == name && m_revision == revision;
}

bool Map::load(istream& in) {
	clear();

	string line;

	/*
	 * Map header: specifies map options (e.g. name, width, etc.)
	 * This section ends when a blank line is encountered.
	 * No blank lines are allowed in this section, but comments are allowed.
	 */
	while (getline(in, line)) {
		if (line[0] == ';' || line[0] == '#') {
			// This line is a comment
			continue;
		}

		StringTokenizer tokenizer(line, " \t", true, 2);
		string option_name;
		tokenizer >> option_name;

		if (option_name.empty()) {
			// Blank line -> end of map header
			break;
		}

		if (option_name == "name") {
			tokenizer >> m_name;
		} else if (option_name == "revision") {
			tokenizer >> m_revision;
		} else if (option_name == "width") {
			tokenizer >> m_width;
		} else if (option_name == "height") {
			tokenizer >> m_height;
		} else {
			// Miscellaneous map option (e.g. game mode, max_players, etc.)
			// Ultimately used for initializing the GameParameters object.
			tokenizer >> m_options[option_name.c_str()];
		}
	}

	/*
	 * Map body: specifies map objects
	 * This section continues for the rest of the file.
	 */
	while (getline(in, line)) {
		// Strip any leading or trailing white space
		strip_leading_trailing_spaces(line);

		// Ignore blank lines and lines starting with # or ; (for comments)
		if (line.empty() || line[0] == '#' || line[0] == ';') {
			continue;
		}

		MapReader reader(line.c_str());
		add_object(reader);
	}

	return true;
}

void Map::clear() {
	m_name.clear();
	m_revision = 0;
	m_width = m_height = 0;
	m_options.clear();
	m_gates.clear();

	while (!m_objects.empty()) {
		MapObject* victim = m_objects.back();
		m_objects.pop_back();
		delete victim;
	}
}

void Map::add_object(MapReader& data) {
	if (MapObject* object = make_map_object(&data)) {
		add_object(object);
	}
}

void Map::add_object(MapObject* object) {
	m_objects.push_back(object);
}

Map::ObjectType	Map::parse_object_type(const char* type_string) {
	if (strcasecmp(type_string, "GATE") == 0)
		return GATE;
	if (strcasecmp(type_string, "SPAWN") == 0)
		return SPAWN_POINT;
	if (strcasecmp(type_string, "OBSTACLE") == 0)
		return OBSTACLE;
	if (strcasecmp(type_string, "DECORATION") == 0 || strcasecmp(type_string, "BACKGROUND") == 0)
		return DECORATION;
	if (strcasecmp(type_string, "REPULSION") == 0)
		return REPULSION;
	if (strcasecmp(type_string, "FORCE") == 0)
		return FORCE_FIELD;
	if (strcasecmp(type_string, "HAZARD") == 0)
		return HAZARD;
	
	return INVALID_OBJECT_TYPE;
}

StringTokenizer& LM::operator>> (StringTokenizer& tok, Map::ObjectType& object_type) {
	if (const char* str = tok.get_next()) {
		object_type = Map::parse_object_type(str);
	} else {
		object_type = Map::INVALID_OBJECT_TYPE;
	}
	return tok;
}

PacketReader& LM::operator>>(PacketReader& packet, Map& map) {
	packet >> map.m_name >> map.m_revision >> map.m_width >> map.m_height;
	return packet;
}

PacketWriter& LM::operator<<(PacketWriter& packet, const Map& map) {
	packet << map.m_name << map.m_revision << map.m_width << map.m_height;
	return packet;
}

