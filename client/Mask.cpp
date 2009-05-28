/*
 * client/Mask.cpp
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

#include "Mask.hpp"
#include <iostream>

using namespace std;

Mask::Mask(Graphic* mask, Graphic* masked) : Graphic() {
	m_mask = mask->clone();
	m_masked = masked->clone();
}

Mask::Mask(const Mask& other) : Graphic(other) {
	m_mask = other.m_mask->clone();
	m_masked = other.m_masked->clone();
}

Mask::~Mask() {
	delete m_mask;
	delete m_masked;
}

Mask* Mask::clone() const {
	return new Mask(*this);
}

Graphic* Mask::get_mask() {
	return m_mask;
}

Graphic* Mask::get_masked() {
	return m_masked;
}

void Mask::set_mask(Graphic* mask) {
	if (mask == NULL) {
		return;
	}
	delete m_mask;
	m_mask = mask->clone();
}

void Mask::set_masked(Graphic* masked) {
	if (masked == NULL) {
		return;
	}
	delete m_masked;
	m_masked = masked->clone();
}

void Mask::draw(const GameWindow* window) const {
	glPushMatrix();
	transform_gl();
	if (!m_mask->is_invisible()) {
		glEnable(GL_ALPHA_TEST);
		bool is_substencil = glIsEnabled(GL_STENCIL_TEST);
		if (!is_substencil) {
			glEnable(GL_STENCIL_TEST);
			glClearStencil(0);
			glClear(GL_STENCIL_BUFFER_BIT);
		}
		glAlphaFunc(GL_NOTEQUAL,0.0);
		glStencilFunc(GL_ALWAYS, 1, 1);
		glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
		m_mask->draw(window);
		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		glDisable(GL_ALPHA_TEST);
		if (!m_masked->is_invisible()) {
			glStencilFunc(GL_EQUAL, 1, 1);
			m_masked->draw(window);
		}
		if (!is_substencil) {
			glDisable(GL_STENCIL_TEST);
		}
	} else {
		if (!m_masked->is_invisible()) {
			m_masked->draw(window);
		}
	}
	glPopMatrix();
}
