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

#include <sstream>

using namespace LM;
using namespace std;

FuzzyLogic::Terminal::Terminal(int cat, int id) {
	m_cat = cat;
	m_id = id;
}

float FuzzyLogic::Terminal::apply(const map<pair<int, int>, float>& values) const {
	// TODO make this return a default?
	ASSERT(values.find(make_pair(m_cat, m_id)) != values.end());
	return values.find(make_pair(m_cat, m_id))->second;
}

FuzzyLogic::And::And(const Rule* lhs, const Rule* rhs) {
	m_lhs = lhs;
	m_rhs = rhs;
}

FuzzyLogic::And::~And() {
	delete m_lhs;
	delete m_rhs;
}

float FuzzyLogic::And::apply(const map<pair<int, int>, float>& values) const {
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

float FuzzyLogic::Or::apply(const map<pair<int, int>, float>& values) const {
	return max<float>(m_lhs->apply(values), m_rhs->apply(values));
}

FuzzyLogic::Not::Not(const Rule* op) {
	m_op = op;
}

FuzzyLogic::Not::~Not() {
	delete m_op;
}

float FuzzyLogic::Not::apply(const map<pair<int, int>, float>& values) const {
	return 1.0f - m_op->apply(values);
}

int FuzzyLogic::add_category(const char* name) {
	m_cats.push_back(FuzzyCategory());
	return m_cats.size() - 1;
}

FuzzyCategory* FuzzyLogic::get_category(int cat) {
	if (m_cats.size() <= (size_t) cat) {
		return NULL;
	}
	return &m_cats[cat];
}

bool FuzzyLogic::load(Configuration* config, const string& section) {
	stringstream s("FuzzyLogic");
	s << "." << section << ".bins";

	ConstIterator<pair<const char*, const char*> > citer = config->get_section(s.str().c_str());
	// TODO put this category somewhere
	FuzzyCategory cat;
	FuzzyCategory::Bin last_bin;
	const char* last_bin_name = "";
	size_t last_bin_len = 0;
	while (citer.has_more()) {
		pair<const char*, const char*> citem = citer.next();
		const char* bin_dot = strchr(citem.first, '.');
		if (bin_dot == NULL) {
			// This is not a bin
			WARN("Fuzzy logic configuration found a non-bin: " << s.str() << "." << citem.first);
			continue;
		}

		// These are guaranteed to be in order, so we can do this
		if (strncmp(last_bin_name, citem.first, bin_dot - citem.first)) {
			// Guarantee it's not null
			if (last_bin_name[0]) {
				// TODO take this output somewhere
				cat.add_bin(last_bin);
			}
			last_bin.clear();
			last_bin_name = citem.first;
			last_bin_len = bin_dot - citem.first;
		}

		++bin_dot;
		if(strcmp(bin_dot, "start") == 0) {
			last_bin.start = atof(citem.second);
		} else if (strcmp(bin_dot, "end") == 0) {
			last_bin.end = atof(citem.second);
		} else if (strcmp(bin_dot, "grade") == 0) {
			last_bin.grade_width = atof(citem.second);
		} else {
			WARN("Fuzzy logic configuration contains unknown attribute: " << s.str() << "." << citem.first);
		}
	}
	cat.add_bin(last_bin);

	// TODO return false if it fails
	return true;
}
