/*
 * ai/FuzzyLogic.cpp
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

#include "FuzzyLogic.hpp"
#include "common/misc.hpp"

using namespace LM;
using namespace std;

FuzzyLogic::Terminal::Terminal(int cat, int id) {
	m_cat = cat;
	m_id = id;
}

float FuzzyLogic::Terminal::apply(const map<int, map<int, float> >& values) const {
	// TODO make this return a default?
	ASSERT(values.find(m_cat) != values.end() && values.find(m_cat)->second.find(m_id) != values.find(m_cat)->second.end());
	return values.find(m_cat)->second.find(m_id)->second;
}

FuzzyLogic::And::And(const Rule* lhs, const Rule* rhs) {
	m_lhs = lhs;
	m_rhs = rhs;
}

FuzzyLogic::And::~And() {
	delete m_lhs;
	delete m_rhs;
}

float FuzzyLogic::And::apply(const map<int, map<int, float> >& values) const {
	return min<float>(m_lhs->apply(values), m_rhs->apply(values));
}

FuzzyLogic::Or::Or(const Rule* lhs, const Rule* rhs) {
	m_lhs = lhs;
	m_rhs = rhs;
}

FuzzyLogic::Or::~Or() {
	delete m_lhs;
	delete m_rhs;
}

float FuzzyLogic::Or::apply(const map<int, map<int, float> >& values) const {
	return max<float>(m_lhs->apply(values), m_rhs->apply(values));
}

FuzzyLogic::Not::Not(const Rule* op) {
	m_op = op;
}

FuzzyLogic::Not::~Not() {
	delete m_op;
}

float FuzzyLogic::Not::apply(const map<int, map<int, float> >& values) const {
	return 1.0f - m_op->apply(values);
}

void FuzzyLogic::add_bin(int cat, int id, float height, float start, float end, float grade_width) {
	m_bins[cat][id] = (Bin){ start, end, height, grade_width };
}
