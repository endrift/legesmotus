/*
 * common/WeaponFile.cpp
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

#include "WeaponFile.hpp"
#include "misc.hpp"
#include <fstream>
#include <istream>
#include <cctype>

using namespace LM;
using namespace std;

bool	WeaponFile::load(const char* name, std::istream& in)
{
	static const WeaponReader	empty_reader;

	clear();
	m_name = name;

	string			current_weapon_line;
	string			line;

	while (getline(in, line)) {
		// Ignore blank lines and lines starting with # or ; (for comments)
		if (line.empty() || line[0] == '#' || line[0] == ';') {
			continue;
		}

		if (line[0] != '\t' && !current_weapon_line.empty()) {
			// Add the line which we were previously reading
			strip_leading_trailing_spaces(current_weapon_line);
			m_weapons.push_back(empty_reader);
			m_weapons.back().init(current_weapon_line.c_str());
			current_weapon_line.clear();
		}

		current_weapon_line.append(line);
	}

	if (!current_weapon_line.empty()) {
		strip_leading_trailing_spaces(current_weapon_line);
		m_weapons.push_back(empty_reader);
		m_weapons.back().init(current_weapon_line.c_str());
	}

	return true;
}


bool	WeaponFile::load_file(const char* name, const char* path)
{
	ifstream		file(path);

	return file && load(name, file);
}


void	WeaponFile::clear()
{
	m_name.clear();
	m_weapons.clear();
}

