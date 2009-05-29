/*
 * common/StringTokenizer.cpp
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

#include "StringTokenizer.hpp"
#include "Point.hpp"
#include <cstring>
#include <cstdlib>
#include <ostream>
#include <limits>

// See .hpp file for extensive comments.

using namespace std;

StringTokenizer::StringTokenizer() {
	m_delimiter = 0;
	m_buffer = NULL;
	m_next_token = NULL;
	m_tokens_left = 0;
}

const char*	StringTokenizer::get_next() {
	if (m_next_token == NULL) {
		// Already at end
		return NULL;
	}

	if (--m_tokens_left == 0) {
		// Last token
		return get_rest();
	}

	// Remember where we started.
	char*		start = m_next_token;

	// Advance until we either hit the end of the data or reach a field separator
	while (*m_next_token != '\0' && *m_next_token != m_delimiter) {
		++m_next_token;
	}
	
	if (*m_next_token == '\0') {
		m_next_token = NULL; // End of packet data -- invalidate the reader
	} else {
		*m_next_token++ = '\0'; // Overwrite the field separator
	}
	
	return start;
}



void	StringTokenizer::init(const char* str, char delimiter) {
	init_from_raw_data(str, strlen(str), delimiter);
}

void	StringTokenizer::init(const char* str, char delimiter, size_t max_tokens) {
	init(str, delimiter);
	m_tokens_left = max_tokens;
}

void	StringTokenizer::init_from_raw_data(const char* str, size_t len, char delimiter) {
	delete[] m_buffer;
	m_buffer = NULL;

	m_delimiter = delimiter;

	// Copy in the data
	m_buffer = new char[len + 1];
	memcpy(m_buffer, str, len);
	m_buffer[len] = '\0';
	// Start at the beginning
	m_next_token = m_buffer;

	m_tokens_left = numeric_limits<size_t>::max();
}

StringTokenizer::~StringTokenizer() {
	delete[] m_buffer;
}

const char*	StringTokenizer::get_rest() const {
	return m_next_token;
}

StringTokenizer&	StringTokenizer::operator>> (bool& b) {
	// The following values are considered true:
	//  any positive integer, "true", "yes", "on"
	// Any other value is considered false.

	const char* p = get_next();
	b = p && (strcasecmp(p, "yes") == 0 || strcasecmp(p, "true") == 0 || strcasecmp(p, "on") == 0 || atoi(p) > 0);
	return *this;
}

StringTokenizer&	StringTokenizer::operator>> (char& c) {
	const char* p = get_next();
	c = p ? p[0] : '\0';
	return *this;
}

StringTokenizer&	StringTokenizer::operator>> (short& i) {
	const char* p = get_next();
	i = p ? short(atoi(p)) : 0;
	return *this;
}

StringTokenizer&	StringTokenizer::operator>> (unsigned short& i) {
	const char* p = get_next();
	i = p ? (unsigned short)atoi(p) : 0;
	return *this;
}

StringTokenizer&	StringTokenizer::operator>> (int& i) {
	const char* p = get_next();
	i = p ? atoi(p) : 0;
	return *this;
}

StringTokenizer&	StringTokenizer::operator>> (unsigned int& i) {
	const char* p = get_next();
	i = p ? (unsigned int)atoi(p) : 0;
	return *this;
}

StringTokenizer&	StringTokenizer::operator>> (long& i) {
	const char* p = get_next();
	i = p ? atol(p) : 0;
	return *this;
}

StringTokenizer&	StringTokenizer::operator>> (unsigned long& i) {
	const char* p = get_next();
	i = p ? strtoul(p, NULL, 10) : 0;
	return *this;
}

StringTokenizer&	StringTokenizer::operator>> (long long& i) {
	const char* p = get_next();
	i = p ? atoll(p) : 0;
	return *this;
}

StringTokenizer&	StringTokenizer::operator>> (unsigned long long& i) {
	const char* p = get_next();
	i = p ? strtoull(p, NULL, 10) : 0;
	return *this;
}

StringTokenizer&	StringTokenizer::operator>> (float& i) {
	const char* p = get_next();
	i = p ? strtof(p, NULL) : 0.0;
	return *this;
}


StringTokenizer&	StringTokenizer::operator>> (double& i) {
	const char* p = get_next();
	i = p ? atof(p) : 0;
	return *this;
}


StringTokenizer&	StringTokenizer::operator>> (string& s) {
	if (const char* p = get_next()) {
		s = p;
	} else {
		s.clear();
	}
	return *this;
}

StringTokenizer&	StringTokenizer::operator>> (Point& point) {
	if (const char* p = get_next()) {
		point.init_from_string(p);
	} else {
		point.clear();
	}
	return *this;
}


