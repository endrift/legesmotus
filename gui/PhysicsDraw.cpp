/*
 * gui/PhysicsDraw.cpp
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

#include "PhysicsDraw.hpp"
#include "common/math.hpp"

using namespace LM;
using namespace std;

PhysicsDraw::Impl::Impl(DrawContext* ctx) {
	SetFlags(0xFF);
	m_ctx = ctx;
}

Color PhysicsDraw::Impl::colorConv(const b2Color& color) {
	return Color(color.r, color.g, color.b, 0.5f);
}

void PhysicsDraw::Impl::DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) {
	float verts[vertexCount*2];
	for (int i = 0; i < vertexCount; ++i) {
		verts[i*2] = vertices[i].x;
		verts[i*2+1] = vertices[i].y;
	}
	m_ctx->set_draw_color(colorConv(color));
	m_ctx->draw_lines(verts, vertexCount, true);
}

void PhysicsDraw::Impl::DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) {
	float verts[vertexCount*2];
	for (int i = 0; i < vertexCount; ++i) {
		verts[i*2] = vertices[i].x;
		verts[i*2+1] = vertices[i].y;
	}
	m_ctx->set_draw_color(colorConv(color));
	m_ctx->draw_polygon(verts, vertexCount);
}

void PhysicsDraw::Impl::DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color) {
	m_ctx->set_draw_color(colorConv(color));
	m_ctx->translate(center.x, center.y);
	m_ctx->draw_arc_line(1, radius, radius, 32);
	m_ctx->translate(-center.x, -center.y);
}

void PhysicsDraw::Impl::DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color) {
	m_ctx->set_draw_color(colorConv(color));
	m_ctx->translate(center.x, center.y);
	m_ctx->draw_arc(1, radius, radius, 32);
	m_ctx->draw_line(0.0f, 0.0f, axis.x, axis.y);
	m_ctx->translate(-center.x, -center.y);
}

void PhysicsDraw::Impl::DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color) {
	m_ctx->set_draw_color(colorConv(color));
	m_ctx->draw_line(p1.x, p1.y, p2.x, p2.y);
}

void PhysicsDraw::Impl::DrawTransform(const b2Transform& xf) {
	// Nothing to do
}

PhysicsDraw::PhysicsDraw(b2World* world) {
	m_world = world;
}

void PhysicsDraw::set_world(b2World* world) {
	m_world = world;
}

void PhysicsDraw::draw(DrawContext* ctx) const {
	if (m_world == NULL) {
		return;
	}
	Impl impl(ctx);
	m_world->SetDebugDraw(&impl);
	ctx->push_transform();
	ctx->scale(PHYSICS_TO_GAME, PHYSICS_TO_GAME);
	m_world->DrawDebugData();
	ctx->pop_transform();
	m_world->SetDebugDraw(NULL);
}
