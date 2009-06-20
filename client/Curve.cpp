/*
 * client/Curve.cpp
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

#include "Curve.hpp"
#include "common/math.hpp"

Curve::Curve(double start, double end) {
	m_start_mapping = start;
	m_end_mapping = end;
}

void Curve::set_start(double start) {
	m_start_mapping = start;
}

void Curve::set_end(double end) {
	m_end_mapping = end;
}

double Curve::get_start() const {
	return m_start_mapping;
}

double Curve::get_end() const {
	return m_end_mapping;
}

double Curve::operator()(double t) const {
	return m_start_mapping + (m_end_mapping-m_start_mapping)*map_progress(t);
}

ConstantCurve::ConstantCurve(double start, double end) : Curve(start, end) {
	// Nothing to do
}

double ConstantCurve::map_progress(double t) const {
	return 1.0;
}

LinearCurve::LinearCurve(double start, double end) : Curve(start, end) {
	// Nothing to do
}

double LinearCurve::map_progress(double t) const {
	return t;
}

SinusoidalCurve::SinusoidalCurve(double start, double end, double frequency, double phase) : Curve(start, end) {
	m_frequency = frequency;
	m_phase = phase;
}

double SinusoidalCurve::map_progress(double t) const {
	return (sin(m_frequency*2.0*M_PI*t + m_phase) + 1.0)*0.5;
}
