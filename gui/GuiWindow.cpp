/*
 * gui/GuiWindow.cpp
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

#include "GuiWindow.hpp"
#include "DrawContext.hpp"

using namespace LM;
using namespace std;

GuiWindow::GuiWindow(Widget* parent) : Widget(parent) {
	m_decoration_color = Color(0.65, 0.85, 1.0, 1);
	m_title_color = Color(0.42, 0.54, 0.64, 1);
	m_background_color = Color(0.42, 0.54, 0.64, 1);

	m_decoration_width = 4.0f;
	m_title_height = 24.0f;

	m_title_widget.set_align(Label::ALIGN_CENTER);
	m_title_widget.set_color(m_decoration_color);

	m_widget = NULL;

	set_width(m_decoration_width * 2.0f + 32.0f);
	set_height(m_decoration_width * 2.0f + m_title_height + 32.0f);
}

void GuiWindow::draw_decoration(DrawContext* ctx) const {
	// Draw frame
	ctx->translate(0, -(get_viewport_height() + m_decoration_width)*0.5);
	ctx->start_clip(); /*{*/
	ctx->draw_rect_fill(get_viewport_width(), m_title_height);
	ctx->finish_clip(); /*}*/
	ctx->translate(0, (get_viewport_height() + m_decoration_width)*0.5);

	ctx->invert_clip(); /*{*/
	ctx->set_draw_color(m_decoration_color);
	ctx->set_blend_mode(DrawContext::BLEND_ADD);
	ctx->draw_rect_fill(get_width(), get_height());
	ctx->invert_clip(); /*}*/

	// Draw title
	ctx->translate(0, -(get_viewport_height() + m_decoration_width)*0.5);
	ctx->start_clip(); /*{*/
	ctx->clip_sub();
	ctx->draw_rect_fill(get_viewport_width(), m_title_height);
	ctx->finish_clip(); /*}*/

	ctx->set_draw_color(m_title_color);
	ctx->set_blend_mode(DrawContext::BLEND_MULTIPLY);
	ctx->draw_rect_fill(get_viewport_width(), m_title_height);

	ctx->set_blend_mode(DrawContext::BLEND_ADD);
	ctx->translate(0, (m_title_widget.get_height() - m_title_height)*0.5 + m_decoration_width);
	m_title_widget.draw(ctx);
	ctx->translate(0, -(m_title_widget.get_height() - m_title_height)*0.5 + m_decoration_width);

	ctx->translate(0, (get_viewport_height() + m_decoration_width)*0.5);

}

void GuiWindow::set_title_text(const std::wstring& title) {
	m_title_widget.set_string(title);
}

void GuiWindow::set_title_text_font(Font* font) {
	m_title_widget.set_font(font);
}

float GuiWindow::get_viewport_width() const {
	return get_width() - 2*m_decoration_width;
}

float GuiWindow::get_viewport_height() const {
	return get_width() - (3*m_decoration_width + m_title_height);
}

void GuiWindow::set_main_widget(Widget* widget) {
	remove_child(m_widget);
	add_child(widget);
	m_widget = widget;
}

void GuiWindow::set_width(float w) {
	Widget::set_width(w);
	if (m_widget != NULL) {
		m_widget->set_width(get_viewport_width());
	}
}

void GuiWindow::set_height(float h) {
	Widget::set_height(h);
	if (m_widget != NULL) {
		m_widget->set_height(get_viewport_height());
	}
}

void GuiWindow::draw(DrawContext* ctx) const {
	ctx->translate(get_x(), get_y());

	// Position for background
	ctx->translate(0, (m_title_height + m_decoration_width)*0.5f);
	// Draw background
	ctx->set_blend_mode(DrawContext::BLEND_MULTIPLY);
	ctx->set_draw_color(m_background_color);
	ctx->draw_rect_fill(get_viewport_width(), get_viewport_height());

	// Clip background
	ctx->start_clip();
	ctx->draw_rect_fill(get_viewport_width(), get_viewport_height());
	ctx->translate(0, -(m_title_height + m_decoration_width)*0.5f);
	ctx->finish_clip();

	// Draw decoration, content
	ctx->invert_clip();
	draw_decoration(ctx);

	if (m_widget != NULL) {
		ctx->push_clip();
		m_widget->draw(ctx);
		ctx->pop_clip();
	}
	ctx->invert_clip();

	// Clean up
	ctx->start_clip();
	ctx->clip_sub();
	ctx->translate(0, (m_title_height + m_decoration_width)*0.5f);
	ctx->draw_rect_fill(get_viewport_width(), get_viewport_height());
	ctx->translate(0, -(m_title_height + m_decoration_width)*0.5f);
	ctx->finish_clip();

	ctx->translate(-get_x(), -(get_y()));
}
