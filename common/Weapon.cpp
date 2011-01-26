/*
 * common/Weapon.cpp
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

#include "Weapon.hpp"
#include "ClientWeapon.hpp"
#include "WeaponReader.hpp"
#include "StandardGun.hpp"
#include "AreaGun.hpp"
#include "misc.hpp"
#include <cstring>

#include <iostream>

using namespace LM;
using namespace std;

Weapon::Weapon() {
	m_clientpart = NULL;
}

Weapon::Weapon(uint32_t id) : m_id(id) {
	m_clientpart = NULL;
}

Weapon::~Weapon() {
	delete m_clientpart;
}

Weapon*	Weapon::new_weapon (WeaponReader& data, ClientWeapon* clientpart) {
	const char* weapon_type = data.get_type().c_str();
	const uint32_t weapon_id = data.get_id();
	Weapon* weapon = NULL;

	if (strcasecmp(weapon_type, "standard") == 0) {
		weapon = new StandardGun(weapon_id);
	} else if (strcasecmp(weapon_type, "spread") == 0) {
		weapon = new StandardGun(weapon_id);
	} else if (strcasecmp(weapon_type, "impact") == 0) {
		weapon = new AreaGun(weapon_id);
	} else if (strcasecmp(weapon_type, "thaw") == 0) {
		weapon = new StandardGun(weapon_id);
	} else if (strcasecmp(weapon_type, "area") == 0) {
		weapon = new AreaGun(weapon_id);
	//} else if (strcasecmp(weapon_type, "charge") == 0) { 
	} else if (strcasecmp(weapon_type, "penetration") == 0)	{
		weapon = new StandardGun(weapon_id);
	} else {
		WARN("Error: unknown weapon type: " << weapon_type);
	}

	if (weapon != NULL) {
		weapon->m_clientpart = clientpart;
		while (data.has_more()) {
			weapon->parse_param(data.get_next());
		}
		weapon->reset();
	}

	return weapon;
}

bool Weapon::parse_param(const char* param_string) {
	if (strncmp(param_string, "name=", 5) == 0) {
		m_name = param_string + 5;
	} else {
		if (m_clientpart != NULL) {
			return m_clientpart->parse_param(param_string, this);
		}
		return false;
	}
	return true;
}

void Weapon::select(Player* player) {
	if (m_clientpart != NULL && player != NULL) {
		m_clientpart->select(player);
	}
}

