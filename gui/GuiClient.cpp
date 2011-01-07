/*
 * gui/GuiClient.cpp
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

#include "GuiClient.hpp"
#include "SDLInputDriver.hpp"
#include "SDLWindow.hpp"
#include "ResourceCache.hpp"
#include "GraphicalPlayer.hpp"
#include "HumanController.hpp"
#include "common/Weapon.hpp"
#include "common/timer.hpp"
#include "Window.hpp"
#include "Bone.hpp"
#include "common/math.hpp"

using namespace LM;
using namespace std;

GuiClient::GuiClient() {
	// TODO move elsewhere
	m_window = SDLWindow::get_instance(1280, 800, 24, Window::FLAG_VSYNC);
	m_cache = new ResourceCache(get_res_directory(), m_window->get_context());
	m_input = new SDLInputDriver;
	m_input->set_sink(this);
	m_gcontrol = new HumanController; // XXX we don't necessarily want one
	m_gcontrol->set_viewport_size(m_window->get_width(), m_window->get_height());
	set_sink(m_gcontrol);
	set_controller(m_gcontrol);

	preload();

	m_window->set_root_widget(&m_root);
	m_view = new GameView("gv", m_cache, m_window->get_width(), m_window->get_height(), 128, &m_root);

	m_debugdraw = new PhysicsDraw;
	#ifdef LM_DEBUG
	m_view->add_child(m_debugdraw, GameView::OVERLAY);
	#endif

	m_map = NULL;
}

GuiClient::~GuiClient() {
	delete m_cache;
	delete m_input;
	delete m_view;

	// Clean up associations to prevent deletion failures
	m_root.set_parent(NULL);

	// TODO destroy window
}

void GuiClient::preload() {
	preload_image("red_head.png");
	preload_image("red_torso.png");
	preload_image("red_frontarm.png");
	preload_image("red_backarm.png");
	preload_image("red_frontleg.png");
	preload_image("red_backleg.png");
	preload_image("blue_head.png");
	preload_image("blue_torso.png");
	preload_image("blue_frontarm.png");
	preload_image("blue_backarm.png");
	preload_image("blue_frontleg.png");
	preload_image("blue_backleg.png");
	preload_image("aim.png");
}

void GuiClient::preload_image(const char* filename) {
	Image img(filename, m_cache, true);
	m_cache->increment<Image>(filename);
	m_preloaded_images.push_back(filename);
}

void GuiClient::cleanup() {
	set_map(NULL);

	for (vector<string>::iterator iter = m_preloaded_images.begin(); iter != m_preloaded_images.end(); ++iter) {
		m_cache->decrement<Image>(*iter);
	}
}

void GuiClient::set_sink(InputSink* input_sink) {
	m_input_sink = input_sink;
}

void GuiClient::add_player(Player* player) {
	Client::add_player(player);
	GraphicalPlayer *gp = static_cast<GraphicalPlayer*>(player);
	m_view->add_child(gp->get_graphic(), GameView::PLAYERS);
}

void GuiClient::set_own_player(uint32_t id) {
	Client::set_own_player(id);
	m_player = static_cast<GraphicalPlayer*>(get_player(id));
}

void GuiClient::remove_player(uint32_t id) {
	Player *p = get_player(id);
	if (p == NULL) {
		return;
	}

	GraphicalPlayer *gp = static_cast<GraphicalPlayer*>(p);
	m_view->remove_child(gp->get_graphic());

	if (m_player != NULL && m_player->get_id() == id) {
		m_player = NULL;
	}

	Client::remove_player(id);
}

void GuiClient::set_map(Map* map) {
	Client::set_map(map);
	if (m_map != NULL) {
		m_view->remove_child(m_map->get_background());
		m_debugdraw->set_world(NULL);
	}
	if (map != NULL) {
		m_map = static_cast<GraphicalMap*>(map);
		m_view->add_child(m_map->get_background(), GameView::BACKGROUND);
		if (get_game() != NULL) {
			m_debugdraw->set_world(get_game()->get_world());
		}
	}
}

GraphicalPlayer* GuiClient::make_player(const char* name, uint32_t id, char team) {
	return new GraphicalPlayer(name, id, team, m_cache);
}

GraphicalMap* GuiClient::make_map() {
	return new GraphicalMap(m_cache);
}

Weapon* GuiClient::make_weapon(WeaponReader& weapon_data) {
	GraphicalWeapon* gw = new GraphicalWeapon(m_cache);
	Weapon* w = Weapon::new_weapon(weapon_data, gw);
	if (w == NULL) {
		delete gw;
	}
	return w;
}

void GuiClient::run() {
	INFO("Beginning running GuiClient...");
	set_running(true);
	// XXX testing code
	Bone crosshair_bone;
	GraphicContainer aim(false, &m_root);
	Sprite crosshair(m_cache->get<Image>("aim.png"));
	crosshair.set_center_x(128);
	crosshair.set_center_y(128);
	crosshair.set_x(512);
	crosshair.get_bone()->set_parent(&crosshair_bone);
	aim.add_graphic(&crosshair);
	aim.set_x(m_window->get_width()/2);
	aim.set_y(m_window->get_height()/2);
	m_view->set_scale_base(1024);
	crosshair_bone.set_scale_x(m_view->get_scale()/4.0f);
	crosshair_bone.set_scale_y(m_view->get_scale()/4.0f);

	IPAddress host;
	resolve_hostname(host, "endrift.com", 16876);
	connect(host);
	// XXX end testing code

	uint64_t last_time = get_ticks();
	while (running()) {
		uint64_t current_time = get_ticks();
		uint64_t diff = current_time - last_time;

		m_input->update();

		// Fudge the current time so that the remaining time between steps is accounted for
		current_time -= step(diff);

		// XXX move to client, game logic
		crosshair_bone.set_rotation(m_gcontrol->get_aim() * RADIANS_TO_DEGREES);
		// XXX end

		// Recenter player
		if (m_player != NULL) {
			m_view->set_offset_x(m_player->get_x() * m_view->get_scale());
			m_view->set_offset_y(m_player->get_y() * m_view->get_scale());
		}

		m_window->redraw();
		last_time = current_time;
	}

	cleanup();
}

void GuiClient::key_pressed(const KeyEvent& event) {
	m_input_sink->key_pressed(event);

	if (event.type == KEY_ESCAPE) {
		set_running(false);
	}
}

void GuiClient::mouse_moved(const MouseMotionEvent& event) {
	m_input_sink->mouse_moved(event);
}

void GuiClient::mouse_clicked(const MouseButtonEvent& event) {
	m_input_sink->mouse_clicked(event);
}

void GuiClient::system_event(const SystemEvent& event) {
	m_input_sink->system_event(event);

	if (event.type == SYSTEM_QUIT) {
		set_running(false);
	}
}
