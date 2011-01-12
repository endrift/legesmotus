/*
 * gui/GraphicalPlayer.cpp
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

#include "GraphicalPlayer.hpp"
#include "Sprite.hpp"
#include "ResourceCache.hpp"
#include "ShaderSet.hpp"
#include "common/math.hpp"
#include <sstream>

using namespace LM;
using namespace std;


GraphicalPlayer::GraphicalPlayer(const char* name, uint32_t id, char team, ResourceCache* cache) : Player(name, id, team), m_graphic_root(true) {
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

	m_graphic_root.set_width(128);
	m_graphic_root.set_height(128);
	m_graphic_root.build_texture(cache);
	m_root_bone.set_x(m_graphic_root.get_width()*0.5);
	m_root_bone.set_y(m_graphic_root.get_height()*0.5);

	m_cache = cache;

	DrawContext* ctx = cache->get_context();
	PixelShader m_blur_shader = ctx->load_pixel_shader(cache->get_root() + "/" + ctx->shader_directory() + "/blur");
	m_blur = ctx->create_shader_set();
	m_blur->attach_shader(m_blur_shader);
	m_blur->link();
	ctx->bind_shader_set(m_blur);
	m_blur->set_variable("d", 0.01f);
	m_blur->set_variable("tex", 0);
	m_blur->set_variable("ksize", 3);
	ctx->unbind_shader_set();

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
	back_arm.set_center_x(29);
	back_arm.set_center_y(32);
	back_arm.set_x(-4);
	back_arm.set_y(-17);
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
	m_graphic_root.add_graphic("front_arm", &front_arm, 4);
	m_graphic_root.add_graphic("back_arm", &back_arm, -2);
	m_graphic_root.add_graphic("front_leg", &front_leg, 2);
	m_graphic_root.add_graphic("back_leg", &back_leg, 1);

	Image dummyi;
	Sprite dummy(&dummyi);
	stringstream gunname;
	dummy.get_bone()->set_parent(m_graphic_root.get_graphic("front_arm")->get_bone());

	gunname.str("gun_");
	gunname << (PART_BACK_HAND|PART_FRONT_ARM|PART_UNFIRED);
	m_graphic_root.add_graphic(gunname.str(), &dummy, 3);

	gunname.str("gun_");
	gunname << (PART_BACK_HAND|PART_FRONT_ARM|PART_FIRED);
	m_graphic_root.add_graphic(gunname.str(), &dummy, 3);

	gunname.str("gun_");
	gunname << (PART_FRONT_HAND|PART_FRONT_ARM|PART_UNFIRED);
	m_graphic_root.add_graphic(gunname.str(), &dummy, 5);

	gunname.str("gun_");
	gunname << (PART_FRONT_HAND|PART_FRONT_ARM|PART_FIRED);
	m_graphic_root.add_graphic(gunname.str(), &dummy, 5);

	dummy.get_bone()->set_parent(m_graphic_root.get_graphic("back_arm")->get_bone());

	gunname.str("gun_");
	gunname << (PART_BACK_HAND|PART_BACK_ARM|PART_UNFIRED);
	m_graphic_root.add_graphic(gunname.str(), &dummy, -3);

	gunname.str("gun_");
	gunname << (PART_BACK_HAND|PART_BACK_ARM|PART_FIRED);
	m_graphic_root.add_graphic(gunname.str(), &dummy, -3);

	gunname.str("gun_");
	gunname << (PART_FRONT_HAND|PART_BACK_ARM|PART_UNFIRED);
	m_graphic_root.add_graphic(gunname.str(), &dummy, -1);

	gunname.str("gun_");
	gunname << (PART_FRONT_HAND|PART_BACK_ARM|PART_FIRED);
	m_graphic_root.add_graphic(gunname.str(), &dummy, -1);
}

GraphicalPlayer::~GraphicalPlayer() {
	delete m_blur;
	m_cache->get_context()->delete_pixel_shader(m_blur_shader);
}

void GraphicalPlayer::update_location() {
	m_graphic_root.set_x(get_x() - m_graphic_root.get_width()*0.5);
	m_graphic_root.set_y(get_y() - m_graphic_root.get_height()*0.5);
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
	Player::set_gun_rotation_degrees(rotation);
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

		get_weapon_graphic(PART_BACK_HAND|PART_BACK_ARM|PART_UNFIRED)->set_invisible(true);
		get_weapon_graphic(PART_FRONT_HAND|PART_BACK_ARM|PART_UNFIRED)->set_invisible(true);
		get_weapon_graphic(PART_BACK_HAND|PART_BACK_ARM|PART_FIRED)->set_invisible(true);
		get_weapon_graphic(PART_FRONT_HAND|PART_BACK_ARM|PART_FIRED)->set_invisible(true);
		get_weapon_graphic(PART_BACK_HAND|PART_FRONT_ARM|PART_UNFIRED)->set_invisible(false);
		get_weapon_graphic(PART_FRONT_HAND|PART_FRONT_ARM|PART_UNFIRED)->set_invisible(false);
		get_weapon_graphic(PART_BACK_HAND|PART_FRONT_ARM|PART_FIRED)->set_invisible(false);
		get_weapon_graphic(PART_FRONT_HAND|PART_FRONT_ARM|PART_FIRED)->set_invisible(false);
	} else {
		m_graphic_root.get_graphic("front_arm")->set_rotation(0);
		m_graphic_root.get_graphic("back_arm")->set_rotation(-rotation + 60);

		get_weapon_graphic(PART_BACK_HAND|PART_FRONT_ARM|PART_UNFIRED)->set_invisible(true);
		get_weapon_graphic(PART_FRONT_HAND|PART_FRONT_ARM|PART_UNFIRED)->set_invisible(true);
		get_weapon_graphic(PART_BACK_HAND|PART_FRONT_ARM|PART_FIRED)->set_invisible(true);
		get_weapon_graphic(PART_FRONT_HAND|PART_FRONT_ARM|PART_FIRED)->set_invisible(true);
		get_weapon_graphic(PART_BACK_HAND|PART_BACK_ARM|PART_UNFIRED)->set_invisible(false);
		get_weapon_graphic(PART_FRONT_HAND|PART_BACK_ARM|PART_UNFIRED)->set_invisible(false);
		get_weapon_graphic(PART_BACK_HAND|PART_BACK_ARM|PART_FIRED)->set_invisible(false);
		get_weapon_graphic(PART_FRONT_HAND|PART_BACK_ARM|PART_FIRED)->set_invisible(false);
	}
}

void GraphicalPlayer::set_is_invisible(bool invisible) {
	Player::set_is_invisible(invisible);
	
	m_graphic_root.set_drawable(!invisible);
}

void GraphicalPlayer::set_is_frozen(bool is_frozen, int64_t freeze_time) {
	Player::set_is_frozen(is_frozen, freeze_time);
	Graphic* graphic = m_graphic_root.get_texture();
	if (is_frozen) {
		graphic->set_shader_set(m_blur);
		graphic->set_color(Color(0.7f, 0.7f, 0.7f));
	} else {
		graphic->set_shader_set(NULL);
		graphic->set_color(Color::WHITE);
	}
}

Graphic* GraphicalPlayer::get_weapon_graphic(int partid) {
	stringstream gunname("gun_");
	gunname << partid;
	return m_graphic_root.get_graphic(gunname.str());
}
