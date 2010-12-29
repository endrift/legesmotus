/*
 * common/Weapon.cpp
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

#include "Weapon.hpp"
#include "WeaponReader.hpp"
#include "StandardGun.hpp"
#include "misc.hpp"
#include <cstring>

#include <iostream>

using namespace LM;
using namespace std;

Weapon::Weapon() {
}

Weapon::Weapon(uint32_t id) : m_id(id) {
}

Weapon*	Weapon::new_weapon (WeaponReader& data) {
	const char* weapon_type = data.get_type().c_str();
	const uint32_t weapon_id = data.get_id();

	if (strcasecmp(weapon_type, "standard") == 0) {
		INFO("Standard.");
		return new StandardGun(weapon_id, data);
	} else if (strcasecmp(weapon_type, "spread") == 0) {
		INFO("Spread.");
		return new StandardGun(weapon_id, data);
	} else if (strcasecmp(weapon_type, "impact") == 0) {
		INFO("Impact.");
	} else if (strcasecmp(weapon_type, "thaw") == 0) {
		INFO("Thaw.");
		return new StandardGun(weapon_id, data);
	} else if (strcasecmp(weapon_type, "area") == 0) {
		INFO("Area.");
	//} else if (strcasecmp(weapon_type, "charge") == 0) { 
	} else if (strcasecmp(weapon_type, "penetration") == 0)	{
		INFO("Penetration.");
		return new StandardGun(weapon_id, data);
	} else {
		WARN("Error: unknown weapon type: " << weapon_type);
	}

	return NULL;
}

bool	Weapon::parse_param(const char* param_string) {
	if (strncmp(param_string, "name=", 5) == 0) {
		m_name = param_string + 5;
	} else if (strncmp(param_string, "hud=", 4) == 0) {
		// This class does not deal with graphics.
	} else if (strncmp(param_string, "normal=", 7) == 0) {
		// This class does not deal with graphics.
	} else if (strncmp(param_string, "firing=", 7) == 0) {
		// This class does not deal with graphics.
	} else if (strncmp(param_string, "impact=", 7) == 0) {
		// This class does not deal with graphics.
	} else {
		return false;
	}
	return true;
}

void	Weapon::select(Player& player) {
	// TODO: Do we need the base class to do something here?
}

