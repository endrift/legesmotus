/*
 * client/TextInput.cpp
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

#include "TextInput.hpp"
#include "GameWindow.hpp"
#include "TextManager.hpp"

using namespace LM;
using namespace std;

TextInput::TextInput(TextManager* manager, double x, double y, unsigned int limit) {
	m_current = string();
	m_prefix = string();
	m_manager = manager;
	m_background = NULL;
	m_bg_scale = false;
	m_bg_padding = 0;
	m_current_sprite = NULL;
	m_prefix_sprite = NULL;
	m_crop_area = NULL;
	m_group = new GraphicGroup;
	m_current_char = 0;
	m_cursor = NULL; // TODO
	m_limit = limit;
	m_window = NULL;
	m_x = x;
	m_y = y;
	recalc();
}

TextInput::~TextInput() {
	if (m_crop_area != NULL) {
		GraphicGroup* group = m_crop_area->get_group();
		group->take_graphic(m_group);
		if (m_window != NULL) {
			m_window->unregister_hud_graphic(m_crop_area);
		}
	} else if (m_window != NULL) {
		m_window->unregister_hud_graphic(m_group);
	}

	if (m_window != NULL) {
		m_window->unregister_hud_graphic(m_background);
	}

	delete m_group;
	delete m_background;
}

void TextInput::recalc() {
	if (m_background == NULL) {
		return;
	}
	m_background->set_center_x(m_bg_padding);
	m_background->set_center_y(m_bg_padding);
	m_background->set_x(m_x);
	m_background->set_y(m_y);
	if (m_prefix_sprite != NULL) {
		if (m_crop_area == NULL) {
			if (m_current_sprite != NULL) {
				m_current_sprite->set_x(m_prefix_sprite->get_image_width());
			}
		} else {
			m_crop_area->set_x(m_prefix_sprite->get_image_width());
		}
	} else if (m_crop_area != NULL) {
		m_crop_area->set_x(0);
	}
	if (m_bg_scale) {
		m_background->set_height(m_manager->get_font()->max_height() + 2*m_bg_padding);
		if (m_crop_area == NULL) {
			if (m_current_sprite != NULL) {
				m_background->set_width(m_current_sprite->get_x() + m_current_sprite->get_image_width() + 2*m_bg_padding);
			} else if (m_prefix_sprite != NULL) {
				m_background->set_width(m_prefix_sprite->get_image_width() + 2*m_bg_padding);
			}
			m_group->set_x(m_x);
			m_group->set_y(m_y);
			m_group->set_center_x(0);
			m_group->set_center_y(0);
		} else {
			m_background->set_width(m_crop_width + m_bg_padding);
			m_group->set_center_x(0);
			m_group->set_center_y(0);
			m_group->set_x(m_x);
			m_group->set_y(m_y);
		}
	}
	if (m_crop_area != NULL) {
		m_crop_area->set_width(m_crop_width - m_crop_area->get_x() + m_bg_padding);
		m_crop_area->set_height(m_manager->get_font()->max_height() + 2*m_bg_padding);
		m_crop_area->set_content_height(m_manager->get_font()->max_height() + 2*m_bg_padding);
		if (m_current_sprite != NULL) {
			m_crop_area->set_content_width(max<double>(m_current_sprite->get_image_width() + 2*m_bg_padding, m_crop_width - m_crop_area->get_x() + m_bg_padding));
		} else {
			m_crop_area->set_content_width(1);
		}
		m_crop_area->set_y(0);
		m_crop_area->set_center_x(0);
		m_crop_area->set_center_y(0);
	}
}

bool TextInput::mouse_button_event(const SDL_MouseButtonEvent& event) {
	// TODO cursor relocation
	(void)(event);
	return true;
}

void TextInput::keyboard_event(const SDL_KeyboardEvent& event) {
	if (event.state == SDL_RELEASED) {
		return;
	}

	switch (event.keysym.sym) {
	case SDLK_BACKSPACE:
		if (m_current.size() > 0) {
			set_value(m_current.substr(0, m_current.size() - 1));
		}
		break;
	default:
		if ((event.keysym.unicode & 0xFF80) == 0 && event.keysym.unicode != 0) {
			// TODO: to utf8
			m_current.append(1, (char)event.keysym.unicode);
			set_value(m_current);
		}
		break;
	}
	// TODO
}

void TextInput::set_value(const std::string& value) {
	m_current = value;
	GraphicGroup* group = m_crop_area == NULL?m_group:m_crop_area->get_group();
	if (m_limit > 0) {
		m_current = m_current.substr(0, m_limit);
	}
	group->remove_graphic("current");
	m_current_sprite = m_manager->render_string(m_current, 0, 0);
	group->give_graphic(m_current_sprite, "current");
	recalc();
	if (m_crop_area != NULL) {
		m_crop_area->set_horiz_scroll_progress(1.0);
	}
}

void TextInput::set_default_value(const std::string& value) {
	m_default = value;
}

void TextInput::set_prefix(const std::string& value) {
	m_prefix = value;
	m_group->remove_graphic("prefix");
	m_prefix_sprite = m_manager->render_string(m_prefix, 0, 0);
	m_group->give_graphic(m_prefix_sprite, "prefix");
	recalc();
}

void TextInput::set_limit(unsigned int limit) {
	m_limit = limit;
	if (limit > 0 && m_current.size() > m_limit) {
		set_value(m_current);
	}
}

void TextInput::set_crop_width(double width) {
	if (m_crop_area == NULL) {
		m_crop_area = new ScrollArea(width, 1, 1, 1);
		m_group->give_graphic(m_crop_area, "crop");
		if (m_current_sprite != NULL) {
			m_group->take_graphic("current");
			m_crop_area->get_group()->give_graphic(m_current_sprite, "current");
		}
	} else {
		m_crop_area->set_width(width);
	}
	m_crop_width = width;
	recalc();
}

void TextInput::set_window(GameWindow* window) {
	if (m_window != NULL) {
		if (m_crop_area != NULL) {
			m_window->unregister_hud_graphic(m_crop_area);
		} else {
			m_window->unregister_hud_graphic(m_group);
		}
		if (m_background != NULL) {
			m_window->unregister_hud_graphic(m_background);
		}
	}
	m_window = window;
	if (m_window != NULL) {
		if (m_crop_area != NULL) {
			m_window->register_hud_graphic(m_crop_area);
		} else {
			m_window->register_hud_graphic(m_group);
		}
		if (m_background != NULL) {
			m_window->register_hud_graphic(m_background);
		}
	}
}

void TextInput::set_background(Graphic* background) {
	if (m_background != NULL && m_window != NULL) {
		m_window->unregister_hud_graphic(m_background);
	}
	delete m_background;
	m_background = background;
	if (m_background != NULL && m_window != NULL) {
		m_window->register_hud_graphic(m_background);
	}
	recalc();
}

void TextInput::set_background_scale(bool enable) {
	m_bg_scale = enable;
	recalc();
}

void TextInput::set_background_padding(double padding) {
	m_bg_padding = padding;
	recalc();
}

string TextInput::get_value() const {
	return m_current;
}

string TextInput::get_default_value() const {
	return m_default;
}

void TextInput::reset() {
	set_value(m_default);
}
