/*
 * client/TextManager.cpp
 *
 * Copyright 2009 - Nathan Partlan, Andrew Ayer, Daniel Schneider, and Jeffrey Pfau
 * 
 */

#include "TextManager.hpp"
#include "GameWindow.hpp"
#include "GraphicGroup.hpp"

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

Graphic* TextManager::place_string(const std::string& text, double x, double y, Align align, Layer layer) {
	Sprite* rendered = m_font->render_string(text);
	Sprite* shadow = NULL;
	if (!rendered) {
		return NULL;
	}
	GraphicGroup* graphic = new GraphicGroup;
	graphic->add_graphic(rendered);
	pair<Sprite*,Sprite*> self(rendered,NULL);
	rendered->set_center_x(0);
	rendered->set_center_y(0);
	if (m_shadow) {
		shadow = new Sprite(*rendered);
		shadow->set_priority(1);
		shadow->set_red_intensity(m_shadow_red);
		shadow->set_green_intensity(m_shadow_green);
		shadow->set_blue_intensity(m_shadow_blue);
		shadow->set_alpha(m_shadow_alpha);
		shadow->set_x(m_shadow_x);
		shadow->set_y(m_shadow_y);
		self.second = shadow;
		graphic->add_graphic(shadow);
	}
	m_owned[graphic] = self;
	m_texts.push_back(graphic);
	rendered->set_priority(0);
	rendered->set_red_intensity(m_active_red);
	rendered->set_green_intensity(m_active_green);
	rendered->set_blue_intensity(m_active_blue);
	rendered->set_alpha(m_active_alpha);
	reposition_string(graphic, x, y, align);
	if (m_window != NULL) {
		switch(layer) {
		case LAYER_HUD:
			m_window->register_hud_graphic(graphic);
			break;
		case LAYER_MAIN:
			m_window->register_graphic(graphic);
			break;
		}
	}
	return graphic;
}

void TextManager::reposition_string(Graphic* text, double x, double y, Align align) {
	switch (align) {
	case LEFT:
		text->set_center_x(0);
		break;
	case CENTER:
		text->set_center_x(m_owned[text].first->get_image_width()/2.0);
		break;
	case RIGHT:
		text->set_center_x(m_owned[text].first->get_image_width());
		break;
	}
	text->set_center_y(0);
	text->set_x(x);
	text->set_y(y);
}

void TextManager::remove_string(Graphic *text) {
	for (vector<Graphic*>::iterator iter = m_texts.begin(); iter != m_texts.end(); ++iter) {
		if (*iter == text) {
			m_texts.erase(iter);
			delete m_owned[text].first;
			delete m_owned[text].second;
			m_owned.erase(text);
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
	for (vector<Graphic*>::iterator iter = m_texts.begin(); iter != m_texts.end(); ++iter) {
		if(m_window != NULL) {
			m_window->unregister_graphic(*iter);
			m_window->unregister_hud_graphic(*iter);
		}
		delete m_owned[*iter].first;
		delete m_owned[*iter].second;
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
