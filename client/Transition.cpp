/*
 * client/Transition.cpp
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

#include "Transition.hpp"
#include "Graphic.hpp"

using namespace LM;

Transition::Transition(Graphic* transitioned, Property property, Curve* curve, uint64_t start, uint64_t duration) {
	m_transed = transitioned;
	m_prop = property;
	m_curve = curve;
	m_start = start;
	m_duration = duration;
	m_curve_owned = false;
	m_graphic_owned = false;
}

Transition::~Transition() {
	if (m_curve_owned) {
		delete m_curve;
	}
	
	if (m_graphic_owned) {
		delete m_transed;
	}
}

void Transition::set_start(uint64_t start) {
	m_start = start;
}

void Transition::set_duration(uint64_t duration) {
	m_duration = duration;
}

void Transition::set_curve(Curve* curve) {
	if (m_curve_owned) {
		delete m_curve;
	}
	m_curve = curve;
}

void Transition::set_curve_ownership(bool owned) {
	m_curve_owned = owned;
}

void Transition::set_graphic_ownership(bool owned) {
	m_graphic_owned = owned;
}

uint64_t Transition::get_start() const {
	return m_start;
}

uint64_t Transition::get_duration() const {
	return m_duration;
}

Curve* Transition::get_curve() {
	return m_curve;
}

Graphic* Transition::get_graphic() {
	return m_transed;
}

bool Transition::get_curve_ownership() const {
	return m_curve_owned;
}

bool Transition::get_graphic_ownership() const {
	return m_graphic_owned;
}

bool Transition::update(uint64_t current) {
	if (m_duration == 0) {
		return true;
	}
	bool passed = false;
	uint64_t progress = current - m_start;
	if (progress >= m_duration) {
		progress = m_duration;
		passed = true;
	} else if (current < m_start) {
		progress = 0;
	}
	(m_transed->*m_prop)((*m_curve)(double(progress)/double(m_duration)));
	return passed;
}

void Transition::change_curve(uint64_t current, Curve* curve, uint64_t duration) {
	if (m_duration != 0) {
		uint64_t progress = current - m_start;
		if (progress >= m_duration) {
			progress = m_duration;
		} else if (current < m_start) {
			progress = 0;
		}
		curve->set_start((*m_curve)(double(progress)/double(m_duration)));
	}
	if (m_curve_owned) {
		delete m_curve;
	}
	m_curve = curve;
	m_start = current;
	if (duration != 0) {
		m_duration = duration;
	}
}
