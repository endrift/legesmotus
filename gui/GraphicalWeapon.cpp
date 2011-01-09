/*
 * gui/GraphicalWeapon.cpp
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

#include "GraphicalWeapon.hpp"

using namespace LM;
using namespace std;

GraphicalWeapon::GraphicalWeapon(ResourceCache* cache) {
	m_cache = cache;
}

bool GraphicalWeapon::parse_param(const char* param_string, Weapon* owner) {
	int type = 0;
	if (strncasecmp(param_string, "normal=", 7) == 0) {
		type |= GraphicalPlayer::PART_UNFIRED;
	} else if (strncasecmp(param_string, "firing=", 7) == 0) {
		type |= GraphicalPlayer::PART_FIRED;
	} else {
		return false;
	}

	StringTokenizer item_tokenizer(param_string + 7, ';');
	while (item_tokenizer.has_more()) {
		int thistype = type;
		GunPart part;
		string item_string;
		string part_image_name;
		string part_type_primary, part_type_secondary;
		item_tokenizer >> item_string;

		StringTokenizer(item_string, ':') >> part.position >> part.rotation >> part_type_primary >> part_type_secondary >> part_image_name;

		if (!part_image_name.empty()) {
			part.image = Image(part_image_name, m_cache, true);
		} else {
			part.image = Image();
		}

		if (part_type_primary == "front") {
			thistype |= GraphicalPlayer::PART_FRONT_ARM;
		} else if (part_type_primary == "back") {
			thistype |= GraphicalPlayer::PART_BACK_ARM;
		}

		if (part_type_secondary == "front") {
			thistype |= GraphicalPlayer::PART_FRONT_HAND;
		} else if (part_type_secondary == "back") {
			thistype |= GraphicalPlayer::PART_BACK_HAND;
		}

		m_gunpart[thistype] = part;
	}

	return true;
}

void GraphicalWeapon::select(Player* player) {
	GraphicalPlayer* p = static_cast<GraphicalPlayer*>(player);
	for (int i = 0; i < GraphicalPlayer::PART_MAX; ++i) {
		Graphic* sprite = p->get_weapon_graphic(i);
		sprite->set_image(m_gunpart[i].image);
		sprite->set_x(m_gunpart[i].position.x - 16);
		sprite->set_y(m_gunpart[i].position.y + 14);
		sprite->set_rotation(m_gunpart[i].rotation);
	}
}
