/*
 * serverscanner/ReadableGenerator.cpp
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

#include "ReadableGenerator.hpp"
#include <sstream>

using namespace LM;
using namespace std;

ReadableGenerator::ReadableGenerator(ostream* outstream) : OutputGenerator(outstream) {
	// Nothing to do
}

void ReadableGenerator::invalidate_row() {
	if (!m_active_cell.empty()) {
		 m_current_row.push(make_pair(m_active_cell, m_list_strings.top()));
		 m_list_strings.pop();
		 m_needs_break = false;
	}

	while (!m_current_row.empty()) {
		pair<string, string> cell = m_current_row.front();
		m_current_row.pop();
		string name = get_column(cell.first);
		size_t spaces = m_longest_prefix - name.length();
		out() << name << ": ";
		for (size_t i = 0; i < spaces; ++i) {
			out() << " ";
		}
		out () << cell.second << "\n";
	}
	out() << endl;
}

void ReadableGenerator::begin() {
	m_longest_prefix = 0;
	m_needs_break = false;
	m_needs_comma = false;
}

void ReadableGenerator::end() {
	// Nothing to do
}
void ReadableGenerator::begin_row() {
	if (m_needs_break) {
		out() << "\n";
		invalidate_row();
	}

	m_longest_prefix = 0;
	m_active_cell.clear();
}

void ReadableGenerator::add_cell(const string& name) {
	m_needs_break = true;
	if (!m_active_cell.empty()) {
		 m_current_row.push(make_pair(m_active_cell, m_list_strings.top()));
		 m_list_strings.pop();
		 m_needs_break = false;
	}
	m_list_strings.push("");
	m_active_cell = name;
	m_longest_prefix = max<size_t>(m_longest_prefix, get_column(name).length());
	m_needs_comma = false;
}

void ReadableGenerator::end_row() {
	invalidate_row();
	m_active_cell.clear();
}

void ReadableGenerator::begin_list() {
	m_list_strings.push("");
	m_needs_comma = false;
	m_needs_break = true;
}

void ReadableGenerator::end_list() {
	string top = m_list_strings.top();
	m_list_strings.pop();
	m_list_strings.top().append(top);
	m_needs_comma = false;
	m_needs_break = false;
}

void ReadableGenerator::add_string(const string& str) {
	stringstream entry;
	if (m_needs_comma) {
		entry << ", ";
	}

	entry << str;
	m_list_strings.top().append(entry.str());
	m_needs_comma = true;
}

void ReadableGenerator::add_int(int num) {
	stringstream entry;
	if (m_needs_comma) {
		entry << ", ";
	}

	entry << num;
	m_list_strings.top().append(entry.str());
	m_needs_comma = true;
}

void ReadableGenerator::add_int(uint64_t num) {
	stringstream entry;
	if (m_needs_comma) {
		entry << ", ";
	}

	entry << num;
	m_list_strings.top().append(entry.str());
	m_needs_comma = true;
}

