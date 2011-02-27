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

FuzzyLogic::Terminal::Terminal(int cat, int bin) {
	m_cat = cat;
	m_id = 0L;
	m_bin = bin;
}
FuzzyLogic::Terminal::Terminal(int cat, long id, int bin) {
	m_cat = cat;
	m_id = id;
	m_bin = bin;
}

float FuzzyLogic::Terminal::apply(const FuzzyEnvironment& values) const {
	return values.get(m_cat, m_id);
}

FuzzyLogic::And::And(const Rule* lhs, const Rule* rhs) {
	m_lhs = lhs;
	m_rhs = rhs;
}

FuzzyLogic::And::~And() {
	delete m_lhs;
	delete m_rhs;
}

float FuzzyLogic::And::apply(const FuzzyEnvironment& values) const {
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

float FuzzyLogic::Or::apply(const FuzzyEnvironment& values) const {
	return max<float>(m_lhs->apply(values), m_rhs->apply(values));
}

FuzzyLogic::Not::Not(const Rule* op) {
	m_op = op;
}

FuzzyLogic::Not::~Not() {
	delete m_op;
}

float FuzzyLogic::Not::apply(const FuzzyEnvironment& values) const {
	return 1.0f - m_op->apply(values);
}

FuzzyLogic::FuzzyLogic(const string& section) : m_section(section) {
	// Nothing to do
}

FuzzyLogic::~FuzzyLogic() {
	for (vector<Rule*>::iterator iter = m_rules.begin(); iter != m_rules.end(); ++iter) {
		delete *iter;
	}
}

int FuzzyLogic::add_category(const string& name) {
	int id = m_cats.size();
	m_cat_ids[name] = id;
	m_cats.push_back(FuzzyCategory());
	return id;
}

FuzzyCategory* FuzzyLogic::get_category(int cat) {
	if (m_cats.size() <= (size_t) cat) {
		return NULL;
	}
	return &m_cats[cat];
}

const FuzzyCategory* FuzzyLogic::get_category(int cat) const {
	if (m_cats.size() <= (size_t) cat) {
		return NULL;
	}
	return &m_cats[cat];
}

int FuzzyLogic::get_category_id(const string& name) const {
	ASSERT(m_cat_ids.find(name) != m_cat_ids.end());
	return m_cat_ids.find(name)->second;
}

bool FuzzyLogic::load_category(const Configuration* config, const string& category) {
	stringstream s;
	s << "FuzzyLogic.";
	if (!m_section.empty()) {
		s << m_section << ".";
	}
	s << category << ".bins";

	ConstIterator<pair<const char*, const char*> > citer = config->get_section(s.str().c_str());
	if (!citer.has_more()) {
		return false;
	}

	FuzzyCategory* cat = get_category(add_category(category));
	ASSERT(cat != NULL);

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
				cat->add_bin(string(last_bin_name).substr(0, last_bin_len), last_bin);
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
	cat->add_bin(last_bin_name, last_bin);

	return true;
}

int FuzzyLogic::add_rule(const string& name, Rule* rule) {
	int id = m_rules.size();
	m_rule_ids[name] = id;
	m_rules.push_back(rule);
	return id;
}

int FuzzyLogic::get_rule_id(const string& name) const {
	ASSERT(m_rule_ids.find(name) != m_rule_ids.end());
	return m_rule_ids.find(name)->second;
}

void FuzzyLogic::apply(FuzzyEnvironment* env) const {
	int i = 0;
	for (vector<FuzzyCategory>::const_iterator iter = m_cats.begin(); iter != m_cats.end(); ++iter, ++i) {
		FuzzyEnvironment::Subenv subenv = env->subset(i);
		iter->apply(subenv);
	}
}

float FuzzyLogic::decide(int rule, FuzzyEnvironment* env) const {
	ASSERT((size_t) rule < m_rules.size());
	return m_rules[rule]->apply(*env);	
}

FuzzyLogic::Terminal* FuzzyLogic::make_terminal(const string& cat, const string& bin) const {
	return make_terminal(cat, 0L, bin);
}

FuzzyLogic::Terminal* FuzzyLogic::make_terminal(const string& cat, long id, const string& bin) const {
	int cat_id = get_category_id(cat);
	return new Terminal(cat_id, id, get_category(cat_id)->get_bin_id(bin));
}

FuzzyLogic::Terminal* FuzzyLogic::make_terminal(const string& cat, void* id, const string& bin) const {
	return make_terminal(cat, (long) id, bin);
}
