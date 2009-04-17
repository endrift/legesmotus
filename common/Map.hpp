/*
 * common/Map.hpp
 *
 * Copyright 2009 - Nathan Partlan, Andrew Ayer, Daniel Schneider, and Jeffrey Pfau
 * 
 */

#ifndef LM_COMMON_MAP_HPP
#define LM_COMMON_MAP_HPP

#include <string>
#include <iosfwd>

class PacketReader;

/*
 * A Map keeps track of things like the map name, dimensions, and spawn points
 * The client should derive a class called GraphicalMap which handles graphics and stuff
 * The server should derive a class called ServerMap which handles spawning
 */
class Map {
public:
	enum {
		OBSTACLE = 1,
		GATE = 2,
		SPAWN_POINT = 3,
		DECORATION = 4
	};

protected:
	std::string	m_name;		// Should be unique
	std::string	m_background;	// Name of the background sprite
	int		m_width;
	int		m_height;

public:
	Map();
	virtual ~Map();

	const char*	get_name() const { return m_name.c_str(); }
	const char*	get_background() const { return m_background.c_str(); }
	int		get_width() const { return m_width; }
	int		get_height() const { return m_height; }
	
	bool		load(std::istream& in);
	bool		load_file(const char* path);

	virtual void	clear();
	virtual void	add_object(PacketReader& data) = 0;
};

#endif
