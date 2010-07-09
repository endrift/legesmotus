/*
 * serverscanner/JsonGenerator.cpp
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

#include "JsonGenerator.hpp"

using namespace LM;
using namespace std;

JsonGenerator::JsonGenerator(ostream* outstream) : OutputGenerator(outstream) {
	m_indentation = 0;
}

void JsonGenerator::begin() {
	m_indentation = 0;
	m_needs_comma = false;
}

void JsonGenerator::end() {
	out() << endl;
}
void JsonGenerator::begin_row() {
	if (m_needs_comma) {
		out() << ",";
		indent();
	}

	out() << "{";
	++m_indentation;
	indent();
	m_needs_comma = false;
}

void JsonGenerator::add_cell(const string& name) {
	if (m_needs_comma) {
		out() << ",";
		indent();
	}

	m_needs_comma = false;
	add_string(name);
	m_needs_comma = false;
	out() << ": ";
}

void JsonGenerator::end_row() {
	--m_indentation;
	indent();
	out() << "}";
	m_needs_comma = true;
}
void JsonGenerator::begin_list() {
	out() << "[";
	++m_indentation;
	indent();
	m_needs_comma = false;
}

void JsonGenerator::end_list() {
	--m_indentation;
	indent();
	out() << "]";
	m_needs_comma = true;
}

void JsonGenerator::add_string(const string& str) {
	if (m_needs_comma) {
		out() << ",";
		indent();
	}

	string escaped;
	for (size_t i = 0, j; i < str.length();) {
		j = str.find_first_of("\"\\", i);
		if (j == string::npos) {
			escaped.append(str, i, str.length());
			break;
		}
		escaped.append(str, i, j - i);
		escaped.append(1, '\\');
		escaped.append(1, str[j]);
		i = j + 1;
	}
	escaped = '"' + escaped + '"';

	out() << escaped;
	m_needs_comma = true;
}

void JsonGenerator::add_int(int num) {
	if (m_needs_comma) {
		out() << ",";
		indent();
	}

	out() << num;
	m_needs_comma = true;
}

void JsonGenerator::add_time(time_t sec) {
	if (m_needs_comma) {
		out() << ",";
		indent();
	}

	out() << sec;
	m_needs_comma = true;
}

void JsonGenerator::add_interval(uint64_t millis) {
	if (m_needs_comma) {
		out() << ",";
		indent();
	}

	out() << '"' << fixed <<  millis*1E-3 << '"';
	m_needs_comma = true;
}

void JsonGenerator::indent() {
	out() << '\n';
	for (int i = 0; i < m_indentation; ++i) {
		out() << '\t';	
	}
}
