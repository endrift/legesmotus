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
#include "common/timer.hpp"
#include "Window.hpp"
#include "Bone.hpp"
#include "common/math.hpp"

using namespace LM;
using namespace std;

GuiClient::GuiClient() {
	// TODO move elsewhere
	m_window = SDLWindow::get_instance(800, 600, 24, Window::FLAG_VSYNC);
	m_cache = new ResourceCache(get_res_directory(), m_window->get_context());
	m_input = new SDLInputDriver;
	m_gcontrol = new HumanController; // XXX we don't necessarily want one
	m_gcontrol->set_viewport_size(m_window->get_width(), m_window->get_height());
	set_input_sink(m_gcontrol);
	set_controller(m_gcontrol);

	preload();
}

GuiClient::~GuiClient() {
	delete m_cache;
	delete m_input;

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
	for (vector<string>::iterator iter = m_preloaded_images.begin(); iter != m_preloaded_images.end(); ++iter) {
		m_cache->decrement<Image>(*iter);
	}
}

void GuiClient::read_input() {
	if (m_input->update() == 0) {
		return;
	}

	KeyEvent ke;
	MouseMotionEvent mme;
	MouseButtonEvent mbe;

	while (m_input->poll_keys(&ke)) {
		m_input_sink->key_pressed(ke);
		if (ke.type == KEY_ESCAPE) {
			set_running(false);
		}
	}

	while (m_input->poll_mouse_motion(&mme)) {
		m_input_sink->mouse_moved(mme);
	}

	while (m_input->poll_mouse_buttons(&mbe)) {
		m_input_sink->mouse_clicked(mbe);
	}
}

void GuiClient::set_input_sink(InputSink* input_sink) {
	m_input_sink = input_sink;
}

GraphicalPlayer* GuiClient::make_player(const char* name, uint32_t id, char team) {
	return new GraphicalPlayer(name, id, team, m_cache);
}

void GuiClient::run() {
	set_running(true);
	// XXX testing code
	Widget root;
	Bone crosshair_bone;
	GraphicContainer aim(&root);
	Sprite crosshair(m_cache->get<Image>("aim.png"));
	crosshair.set_center_x(32);
	crosshair.set_center_y(32);
	crosshair.set_x(128);
	crosshair.get_bone()->set_parent(&crosshair_bone);
	aim.add_graphic(&crosshair);
	GraphicalPlayer a_player("foo", 0, 'B', m_cache);
	a_player.get_graphic()->set_parent(&root);
	m_window->set_root_widget(&root);
	root.set_x(400);
	root.set_y(300);
	a_player.set_rotational_vel(60);
	// XXX end testing code

	uint64_t last_time = get_ticks();
	while (running()) {
		uint64_t current_time = get_ticks();
		uint64_t diff = current_time - last_time;

		read_input();

		step(diff);

		// XXX move to client, game logic
		a_player.update_rotation(diff/1000.0f);
		a_player.set_gun_rotation_radians(m_gcontrol->get_aim());
		crosshair_bone.set_rotation(m_gcontrol->get_aim() * RADIANS_TO_DEGREES);
		// XXX end

		m_window->redraw();
		last_time = current_time;
	}

	cleanup();
}
