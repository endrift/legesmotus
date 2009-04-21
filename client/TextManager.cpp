/*
 * client/TextManager.cpp
 *
 * Copyright 2009 - Nathan Partlan, Andrew Ayer, Daniel Schneider, and Jeffrey Pfau
 * 
 */

#include "TextManager.hpp"
#include "GameWindow.hpp"

using namespace std;

TextManager::TextManager(Font* font) {
	m_font = font;
	m_active_red = 1.0;
	m_active_green = 1.0;
	m_active_blue = 1.0;
	m_active_alpha = 1.0;
}

TextManager::~TextManager() {
	remove_all_strings();
}

Sprite* TextManager::place_string(const std::string& text, double x, double y, Align align, Layer layer, GameWindow* window) {
	Sprite* rendered = m_font->render_string(text);
	if (!rendered) {
		return NULL;
	}
	m_texts.push_back(rendered);
	rendered->set_red_intensity(m_active_red);
	rendered->set_green_intensity(m_active_green);
	rendered->set_blue_intensity(m_active_blue);
	rendered->set_alpha(m_active_alpha);
	reposition_string(rendered,x,y,align);
	if (window != NULL) {
		switch(layer) {
		case LAYER_HUD:
			window->register_hud_graphic(rendered);
			break;
		case LAYER_MAIN:
			window->register_graphic(rendered);
			break;
		}
	}
	return rendered;
}

void TextManager::reposition_string(Sprite* text, double x, double y, Align align) {
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

void TextManager::remove_string(Sprite *text, GameWindow* window) {
	for (vector<Sprite*>::iterator iter = m_texts.begin(); iter != m_texts.end(); ++iter) {
		if (*iter == text) {
			m_texts.erase(iter);
			break;
		}
	}
	if (window != NULL) {
		window->unregister_graphic(text);
		window->unregister_hud_graphic(text);
	}
	delete text;
}
	
void TextManager::remove_all_strings(GameWindow* window) {
	for (vector<Sprite*>::iterator iter = m_texts.begin(); iter != m_texts.end(); ++iter) {
		if(window != NULL) {
			window->unregister_graphic(*iter);
			window->unregister_hud_graphic(*iter);
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
