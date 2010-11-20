/*
 * gui/GraphicalPlayer.cpp
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

#include "GraphicalPlayer.hpp"
#include "Sprite.hpp"
#include "ResourceCache.hpp"
#include "common/math.hpp"

using namespace LM;
using namespace std;

GraphicalPlayer::GraphicalPlayer(const char* name, uint32_t id, char team, ResourceCache* cache) : Player(name, id, team) {
	string color;
	if (team == 'A') {
		color = "blue";
	} else if (team == 'B') {
		color = "red";
	}

	Sprite head(cache->get<Image>(color + "_head.png"));
	Sprite torso(cache->get<Image>(color + "_torso.png"));
	Sprite front_arm(cache->get<Image>(color + "_frontarm.png"));
	Sprite back_arm(cache->get<Image>(color + "_backarm.png"));
	Sprite front_leg(cache->get<Image>(color + "_frontleg.png"));
	Sprite back_leg(cache->get<Image>(color + "_backleg.png"));
    
	torso.get_bone()->set_parent(&m_root_bone);
	torso.set_center_x(32);
	torso.set_center_y(48);
	head.get_bone()->set_parent(&m_root_bone);
	head.set_center_x(35);
	head.set_center_y(23);
	head.set_x(3);
	head.set_y(-25);
	front_arm.get_bone()->set_parent(&m_root_bone);
	front_arm.set_center_x(46);
	front_arm.set_center_y(30);
	front_arm.set_x(14);
	front_arm.set_y(-18);
	back_arm.get_bone()->set_parent(&m_root_bone);
	back_arm.set_center_x(27);
	back_arm.set_center_y(29);
	back_arm.set_x(-5);
	back_arm.set_y(-19);
	front_leg.get_bone()->set_parent(&m_root_bone);
	front_leg.set_center_x(37);
	front_leg.set_center_y(60);
	front_leg.set_x(5);
	front_leg.set_y(12);
	back_leg.get_bone()->set_parent(&m_root_bone);
	back_leg.set_center_x(26);
	back_leg.set_center_y(60);
	back_leg.set_x(-6);
	back_leg.set_y(12);
	m_graphic_root.add_graphic("head", &head, 1);
	m_graphic_root.add_graphic("torso", &torso, 0);
	m_graphic_root.add_graphic("front_arm", &front_arm, 3);
	m_graphic_root.add_graphic("back_arm", &back_arm, -2);
	m_graphic_root.add_graphic("front_leg", &front_leg, 2);
	m_graphic_root.add_graphic("back_leg", &back_leg, 1);
}

GraphicalPlayer::~GraphicalPlayer() {
}

void GraphicalPlayer::update_location() {
	m_graphic_root.set_x(get_x());
	m_graphic_root.set_y(get_y());
	m_root_bone.set_rotation(get_rotation_degrees());
}

GraphicContainer* GraphicalPlayer::get_graphic() {
	return &m_graphic_root;
}

Bone* GraphicalPlayer::get_bone() {
	return &m_root_bone;
}

void GraphicalPlayer::set_position(float x, float y) {
	Player::set_position(x, y);
}

void GraphicalPlayer::set_rotation_degrees(float rotation) {
	Player::set_rotation_degrees(rotation);
}

void GraphicalPlayer::set_gun_rotation_degrees(float rotation) {
	// TODO un-hardcode angle
	// Red team is right-handed, blue team is left-handed
	bool handedness = true;
	int flip = 1;
	if (get_team() != 'A') {
		handedness = false;
	}
	rotation -= get_rotation_degrees();
	rotation = get_normalized_angle(rotation);

	if ((rotation > 90 && rotation < 270) || rotation < -90) {
		handedness = get_team() == 'A';
	} else {
		handedness = get_team() != 'A';
		flip = -flip;
	}

	if (get_team() != 'A') {
		rotation = -rotation + 180;
	}

	m_root_bone.set_scale_x(flip);
	if (handedness) {
		m_graphic_root.get_graphic("front_arm")->set_rotation(rotation - 120);
		m_graphic_root.get_graphic("back_arm")->set_rotation(0);
	} else {
		m_graphic_root.get_graphic("front_arm")->set_rotation(0);
		m_graphic_root.get_graphic("back_arm")->set_rotation(-rotation + 60);
	}
}
