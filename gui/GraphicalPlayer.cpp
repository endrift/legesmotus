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

using namespace LM;
using namespace std;

GraphicalPlayer::GraphicalPlayer(const char* name, uint32_t id, char team, ResourceCache* cache) : Player(name, id, team) {
	string color;
	if (team == 'A') {
		color = "blue";
	} else if (team == 'B') {
		color = "red";
	}

	m_head = new Sprite(cache->get<Image>(color + "_head.png"));
	m_torso = new Sprite(cache->get<Image>(color + "_torso.png"));
	m_front_arm = new Sprite(cache->get<Image>(color + "_frontarm.png"));
	m_back_arm = new Sprite(cache->get<Image>(color + "_backarm.png"));
	m_front_leg = new Sprite(cache->get<Image>(color + "_frontleg.png"));
	m_back_leg = new Sprite(cache->get<Image>(color + "_backleg.png"));

	m_torso->get_bone()->set_parent(&m_root_bone);
	m_torso->set_center_x(32);
	m_torso->set_center_y(48);
	m_head->get_bone()->set_parent(&m_root_bone);
	m_head->set_center_x(35);
	m_head->set_center_y(23);
	m_head->set_x(3);
	m_head->set_y(-25);
	m_front_arm->get_bone()->set_parent(&m_root_bone);
	m_front_arm->set_center_x(46);
	m_front_arm->set_center_y(30);
	m_front_arm->set_x(14);
	m_front_arm->set_y(-18);
	m_back_arm->get_bone()->set_parent(&m_root_bone);
	m_back_arm->set_center_x(27);
	m_back_arm->set_center_y(29);
	m_back_arm->set_x(-5);
	m_back_arm->set_y(-19);
	m_front_leg->get_bone()->set_parent(&m_root_bone);
	m_front_leg->set_center_x(37);
	m_front_leg->set_center_y(60);
	m_front_leg->set_x(5);
	m_front_leg->set_y(12);
	m_back_leg->get_bone()->set_parent(&m_root_bone);
	m_back_leg->set_center_x(26);
	m_back_leg->set_center_y(60);
	m_back_leg->set_x(-6);
	m_back_leg->set_y(12);
	m_graphic_root.add_graphic("head", m_head, 1);
	m_graphic_root.add_graphic("torso", m_torso, 0);
	m_graphic_root.add_graphic("front_arm", m_front_arm, 3);
	m_graphic_root.add_graphic("back_arm", m_back_arm, -2);
	m_graphic_root.add_graphic("front_leg", m_front_leg, 2);
	m_graphic_root.add_graphic("back_leg", m_back_leg, 1);
}

GraphicalPlayer::~GraphicalPlayer() {
	delete m_head;
	delete m_torso;
	delete m_front_arm;
	delete m_back_arm;
	delete m_front_leg;
	delete m_back_leg;
}

GraphicContainer* GraphicalPlayer::get_graphic() {
	return &m_graphic_root;
}

Bone* GraphicalPlayer::get_bone() {
	return &m_root_bone;
}

void GraphicalPlayer::set_x(double x) {
	Player::set_x(x);
	m_graphic_root.set_x(x);
}

void GraphicalPlayer::set_y(double y) {
	Player::set_y(y);
	m_graphic_root.set_y(y);
}
