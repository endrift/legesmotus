/*
 * client/Curve.cpp
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

#include "Curve.hpp"
#include "common/math.hpp"

using namespace LM;

Curve::Curve(float start, float end) {
	m_start_mapping = start;
	m_end_mapping = end;
}

void Curve::set_start(float start) {
	m_start_mapping = start;
}

void Curve::set_end(float end) {
	m_end_mapping = end;
}

float Curve::get_start() const {
	return m_start_mapping;
}

float Curve::get_end() const {
	return m_end_mapping;
}

float Curve::operator()(float t) const {
	return m_start_mapping + (m_end_mapping-m_start_mapping)*map_progress(t);
}

ConstantCurve::ConstantCurve(float start, float end) : Curve(start, end) {
	// Nothing to do
}

float ConstantCurve::map_progress(float t) const {
	return 1.0;
}

LinearCurve::LinearCurve(float start, float end) : Curve(start, end) {
	// Nothing to do
}

float LinearCurve::map_progress(float t) const {
	return t;
}

SinusoidalCurve::SinusoidalCurve(float start, float end, float frequency, float phase) : Curve(start, end) {
	m_frequency = frequency;
	m_phase = phase;
}

float SinusoidalCurve::map_progress(float t) const {
	return (sinf(m_frequency*2.0*M_PI*t + m_phase) + 1.0)*0.5;
}

LogisticCurve::LogisticCurve(float start, float end, float width) : Curve(start, end) {
	m_coeff = 1 + powf(M_E, width*-0.5);
	m_width = width;
}

float LogisticCurve::map_progress(float t) const {
	return m_coeff/(1 + powf(M_E, m_width*(0.5 - t)));
}
