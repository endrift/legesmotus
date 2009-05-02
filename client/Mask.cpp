/*
 * client/Mask.cpp
 *
 * Copyright 2009 - Nathan Partlan, Andrew Ayer, Daniel Schneider, and Jeffrey Pfau
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
