/*
 * serverscanner/OutputGenerator.cpp
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

#include "OutputGenerator.hpp"

using namespace LM;
using namespace std;

OutputGenerator::OutputGenerator(ostream* out) {
	m_out = out;
}

void OutputGenerator::begin() {
	m_indentation = 0;
	m_needs_comma = false;
	begin_dict();
}

void OutputGenerator::end() {
	end_dict();
	(*m_out) << endl;
}
void OutputGenerator::begin_dict() {
	if (m_needs_comma) {
		(*m_out) << ",";
		indent();
	}

	(*m_out) << "{";
	++m_indentation;
	indent();
	m_needs_comma = false;
}

void OutputGenerator::add_dict_entry(const string& name) {
	if (m_needs_comma) {
		(*m_out) << ",";
		indent();
	}

	m_needs_comma = false;
	add_string(name);
	m_needs_comma = false;
	(*m_out) << ": ";
}

void OutputGenerator::end_dict() {
	--m_indentation;
	indent();
	(*m_out) << "}";
	m_needs_comma = true;
}
void OutputGenerator::begin_list() {
	(*m_out) << "[";
	++m_indentation;
	indent();
	m_needs_comma = false;
}

void OutputGenerator::end_list() {
	--m_indentation;
	indent();
	(*m_out) << "]";
	m_needs_comma = true;
}

void OutputGenerator::add_string(const string& str) {
	if (m_needs_comma) {
		(*m_out) << ",";
		indent();
	}

	string escaped = str;
	// TODO escape;
	escaped = '"' + escaped + '"';

	(*m_out) << escaped;
	m_needs_comma = true;
}

void OutputGenerator::add_int(int num) {
	if (m_needs_comma) {
		(*m_out) << ",";
		indent();
	}

	(*m_out) << num;
	m_needs_comma = true;
}

void OutputGenerator::add_int(uint64_t num) {
	if (m_needs_comma) {
		(*m_out) << ",";
		indent();
	}

	(*m_out) << '"' <<  num << '"';
	m_needs_comma = true;
}

void OutputGenerator::indent() {
	(*m_out) << '\n';
	for (int i = 0; i < m_indentation; ++i) {
		(*m_out) << '\t';	
	}
}
