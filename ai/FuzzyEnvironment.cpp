/*
 * ai/FuzzyEnvironment.cpp
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

#include "FuzzyEnvironment.hpp"

using namespace LM;
using namespace std;

FuzzyEnvironment::Subenv::Subenv(FuzzyEnvironment* parent, int category) {
	m_e = parent;
	m_cat = category;
}

void FuzzyEnvironment::Subenv::set(long id, int bin, float value) {
	m_e->set(m_cat, id, bin, value);
}

float FuzzyEnvironment::Subenv::get(long id, int bin) const {
	return m_e->get(m_cat, id, bin);
}

ConstIterator<pair<long, float> > FuzzyEnvironment::Subenv::get_input() const {
	return m_e->get_input(m_cat);
}

void FuzzyEnvironment::Subenv::clear() {
	m_e->clear(m_cat);
}

void FuzzyEnvironment::set(int cat, int bin, float value) {
	m_env[cat][0L][bin] = value;
}

void FuzzyEnvironment::set(int cat, long id, int bin, float value) {
	m_env[cat][id][bin] = value;
}

float FuzzyEnvironment::get(int cat, int bin) const {
	return m_env.find(cat)->second.find(0L)->second.find(bin)->second;
}

float FuzzyEnvironment::get(int cat, long id, int bin) const {
	return m_env.find(cat)->second.find(id)->second.find(bin)->second;
}

void FuzzyEnvironment::set_input(int cat, float value) {
	m_input[cat].clear();
	m_input[cat][0L] = value;
}

void FuzzyEnvironment::set_input(int cat, const map<long, float>& input) {
	m_input[cat] = input;
}

void FuzzyEnvironment::add_input(int cat, long id, float value) {
	m_input[cat][id] = value;
}

void FuzzyEnvironment::add_input(int cat, void* id, float value) {
	m_input[cat][(long) id] = value;
}

ConstIterator<pair<long, float> > FuzzyEnvironment::get_input(int cat) const {
	return ConstIterator<pair<long, float> >(new ConstStdMapIterator<long, float>(&m_input.find(cat)->second));
}

void FuzzyEnvironment::clear() {
	m_env.clear();
	m_input.clear();
}

void FuzzyEnvironment::clear(int cat) {
	if (m_env.find(cat) != m_env.end()) {
		m_env[cat].clear();
	}
	if (m_input.find(cat) != m_input.end()) {
		m_input[cat].clear();
	}
}

FuzzyEnvironment::Subenv FuzzyEnvironment::subset(int cat) {
	return Subenv(this, cat);
}
