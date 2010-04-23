/*
 * client/TextManager.cpp
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

#include "TextManager.hpp"
#include "GameWindow.hpp"
#include "GraphicGroup.hpp"
#include "common/Exception.hpp"
#include <iostream>

using namespace LM;
using namespace std;

TextManager::TextManager(Font* font, GameWindow* window) {
	m_font = font;
	m_active_red = 1.0;
	m_active_green = 1.0;
	m_active_blue = 1.0;
	m_active_alpha = 1.0;
	m_shadow_red = 0.0;
	m_shadow_green = 0.0;
	m_shadow_blue = 0.0;
	m_shadow_alpha = 1.0;
	m_shadow_x = 1.0;
	m_shadow_y = 1.0;
	m_shadow_kernel = NULL;
	m_shadow = false;
	m_window = window;
}

TextManager::~TextManager() {
	remove_all_strings();
	delete m_shadow_kernel;
}

Text* TextManager::render_string(const string& text, double x, double y, Align align) {
	Text *rendered;
	try {
		rendered = new Text(text, m_font, m_shadow?m_shadow_kernel:NULL);
	} catch(const Exception& e) {
		cerr << "Error rendering string: " << e.what() << endl;
		return NULL;
	}
	rendered->set_color(m_active_red, m_active_green, m_active_blue);
	rendered->set_alpha(m_active_alpha);
	if (m_shadow) {
		rendered->set_shadow(true);
		rendered->set_shadow_color(m_shadow_red, m_shadow_green, m_shadow_blue);
		rendered->set_shadow_alpha(m_shadow_alpha);
		rendered->set_shadow_offset(m_shadow_x, m_shadow_y);
	}
	reposition_string(rendered, x, y, align);

	return rendered;
}

Text* TextManager::place_string(const string& text, double x, double y, Align align, Layer layer, int priority) {
	Text* rendered = render_string(text, x, y, align);
	if (rendered == NULL) {
		return NULL;
	}
	rendered->set_priority(priority);
	m_texts.push_back(rendered);
	if (m_window != NULL) {
		switch(layer) {
		case LAYER_HUD:
			m_window->register_hud_graphic(rendered);
			break;
		case LAYER_MAIN:
			m_window->register_graphic(rendered);
			break;
		}
	}
	return rendered;
}

void TextManager::reposition_string(Graphic* text, double x, double y, Align align) {
	switch (align) {
	case LEFT:
		text->set_center_x(0);
		break;
	case CENTER:
		text->set_center_x(text->get_image_width()/2.0);
		break;
	case RIGHT:
		text->set_center_x(text->get_image_width());
		break;
	}
	text->set_center_y(0);
	text->set_x(x);
	text->set_y(y);
}

void TextManager::remove_string(Graphic *text) {
	for (vector<Text*>::iterator iter = m_texts.begin(); iter != m_texts.end(); ++iter) {
		if (*iter == text) {
			m_texts.erase(iter);
			break;
		}
	}
	if (m_window != NULL) {
		m_window->unregister_graphic(text);
		m_window->unregister_hud_graphic(text);
	}
	delete text;
}
	
void TextManager::remove_all_strings() {
	for (vector<Text*>::iterator iter = m_texts.begin(); iter != m_texts.end(); ++iter) {
		if(m_window != NULL) {
			m_window->unregister_graphic(*iter);
			m_window->unregister_hud_graphic(*iter);
		}
		delete *iter;
	}
	m_texts.clear();
}

void TextManager::set_active_color(double r, double g, double b) {
	m_active_red = r;
	m_active_green = g;
	m_active_blue = b;
}

void TextManager::set_active_color(Color c) {
	m_active_red = c.r;
	m_active_green = c.g;
	m_active_blue = c.b;
}

void TextManager::set_active_alpha(double a) {
	m_active_alpha = a;
}

void TextManager::set_active_font(Font* font) {
	m_font = font;
}

void TextManager::set_shadow_color(double r, double g, double b) {
	m_shadow_red = r;
	m_shadow_green = g;
	m_shadow_blue = b;
}

void TextManager::set_shadow_color(Color c) {
	m_shadow_red = c.r;
	m_shadow_green = c.g;
	m_shadow_blue = c.b;
}

void TextManager::set_shadow_alpha(double a) {
	m_shadow_alpha = a;
}

void TextManager::set_shadow_offset(double x, double y) {
	m_shadow_x = x;
	m_shadow_y = y;
}

void TextManager::set_shadow_convolve(Curve* curve, int diameter, double normalization) {
	m_shadow_kernel = new ConvolveKernel(curve, diameter, diameter, normalization);
}

void TextManager::set_shadow(bool enable) {
	m_shadow = enable;
}

void TextManager::set_window(GameWindow* window) {
	m_window = window;
}

Font* TextManager::get_font() {
	return m_font;
}
