/*
 * client/Weapon.cpp
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

#include "Weapon.hpp"
#include "common/WeaponReader.hpp"
#include <cstring>

#include "GameController.hpp"
#include "StandardGun.hpp"
#include "ImpactCannon.hpp"
#include "SpreadGun.hpp"
#include "ThawGun.hpp"

using namespace LM;
using namespace std;

Weapon::Weapon() {
}

Weapon::Weapon(const char* id) : m_id(id) {
}

Weapon*	Weapon::new_weapon (WeaponReader& data) {
	const char*	weapon_type = data.get_type().c_str();
	const char*	weapon_id = data.get_id().c_str();

	if (strcasecmp(weapon_type, "standard") == 0)	{ return new StandardGun(weapon_id, data); }
	if (strcasecmp(weapon_type, "spread") == 0)	{ return new SpreadGun(weapon_id, data); }
	if (strcasecmp(weapon_type, "impact") == 0)	{ return new ImpactCannon(weapon_id, data); }
	if (strcasecmp(weapon_type, "thaw") == 0)	{ return new ThawGun(weapon_id, data); }

	return NULL;
}


bool	Weapon::parse_param(const char* param_string) {
	if (strncmp(param_string, "name=", 5) == 0) {
		m_name = param_string + 5;
	} else if (strncmp(param_string, "hud=", 4) == 0) {
		m_hud_graphic = param_string + 4;
	} else if (strncmp(param_string, "normal=", 7) == 0) {
		m_normal_graphic_info = param_string + 7;
	} else if (strncmp(param_string, "firing=", 7) == 0) {
		m_firing_graphic_info = param_string + 7;
	} else if (strncmp(param_string, "impact=", 7) == 0) {
		m_impact_graphic = param_string + 7;
	} else {
		return false;
	}
	return true;
}

void	Weapon::select(Player& player, GameController& gc) {
	gc.register_front_arm_graphic(player, m_normal_graphic_info.c_str(), m_firing_graphic_info.c_str());
}

