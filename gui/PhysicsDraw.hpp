/*
 * gui/PhysicsDraw.hpp
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

#ifndef LM_GUI_PHYSICSDRAW_HPP
#define LM_GUI_PHYSICSDRAW_HPP

#include "Widget.hpp"
#include "common/misc.hpp"
#include "common/physics.hpp"

namespace LM {
	class PhysicsDraw : public Widget {
	private:
		class Impl : public b2DebugDraw {
		private:
			Color colorConv(const b2Color& color);
			DrawContext* m_ctx;

		public:
			Impl(DrawContext* ctx);

			virtual void DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color);
			virtual void DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color);
			virtual void DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color);
			virtual void DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color);
			virtual void DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color);
			virtual void DrawTransform(const b2Transform& xf);
		};

		b2World* m_world;

	public:
		PhysicsDraw(b2World* world = NULL);

		void set_world(b2World* world);

		virtual void draw(DrawContext* ctx) const;
	};
}

#endif
