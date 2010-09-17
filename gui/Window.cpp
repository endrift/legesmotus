/*
 * gui/Window.cpp
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

#include "Window.hpp"
#include "DrawContext.hpp"
#include "Widget.hpp"

using namespace LM;
using namespace std;

Window* Window::m_instance = NULL;

void Window::set_instance(Window* instance) {
	m_instance = instance;
}

Window::Window(int width, int height, int depth, int flags) {
	m_width = width;
	m_height = height;
	m_depth = depth;

	m_fullscreen = flags & FLAG_FULLSCREEN;
}

Window::~Window() {
	// Nothing to do
}

void Window::set_width(int width) {
	m_width = width;
}

void Window::set_height(int height) {
	m_height = height;
}

void Window::set_depth(int depth) {
	m_depth = depth;
}

Window* Window::get_instance() {
	return m_instance;
}

void Window::destroy_instance() {
	delete m_instance;
	m_instance = NULL;
}

void Window::set_icon(Image* icon) {
	if (m_instance != NULL) {
		m_instance->set_icon_internal(icon);
	}
}

int Window::get_width() const {
	return m_width;
}

int Window::get_height() const {
	return m_height;
}

int Window::get_depth() const {
	return m_depth;
}

bool Window::is_fullscreen() const {
	return m_fullscreen;
}

void Window::set_root_widget(Widget* root) {
	get_context()->set_root_widget(root);
}

Widget* Window::get_root_widget() {
	return get_context()->get_root_widget();
}

void Window::redraw() {
	get_context()->redraw();
}

VmodeNotSupportedException::VmodeNotSupportedException(const string& message, int width, int height, int depth, bool fullscreen) : Exception(message) {
	m_width = width;
	m_height = height;
	m_depth = depth;
	m_fullscreen = fullscreen;
}
