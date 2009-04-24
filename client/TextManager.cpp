/*
 * client/TextManager.cpp
 *
 * Copyright 2009 - Nathan Partlan, Andrew Ayer, Daniel Schneider, and Jeffrey Pfau
 * 
 */

#include "TextManager.hpp"
#include "GameWindow.hpp"
#include "GraphicGroup.hpp"
#include "common/LMException.hpp"
#include <iostream>

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
	m_shadow = false;
	m_window = window;
}

TextManager::~TextManager() {
	remove_all_strings();
}

Text* TextManager::place_string(const std::string& text, double x, double y, Align align, Layer layer) {
	Text *rendered;
	try {
		rendered = new Text(text, m_font);
	} catch(LMException e) {
		cerr << "Error placing string: " << e.what() << endl;
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

void TextManager::set_shadow_alpha(double a) {
	m_shadow_alpha = a;
}

void TextManager::set_shadow_offset(double x, double y) {
	m_shadow_x = x;
	m_shadow_y = y;
}

void TextManager::set_shadow(bool enable) {
	m_shadow = enable;
}

void TextManager::set_window(GameWindow* window) {
	m_window = window;
}
