/*
 * common/StringTokenizer.cpp
 *
 * Copyright 2009 - Nathan Partlan, Andrew Ayer, Daniel Schneider, and Jeffrey Pfau
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

StringTokenizer&	StringTokenizer::operator>> (bool& b)
{
	// True if:
	//  - "yes"
	//  - "true"
	//  - "on"
	//  - positive integer

	const char*	p = get_next();

	b = p && (strcasecmp(p, "yes") == 0 || strcasecmp(p, "true") == 0 || strcasecmp(p, "on") == 0 || atoi(p) > 0);

	return *this;
}


StringTokenizer&	StringTokenizer::operator>> (char& c)
{
	const char*	p = get_next();
	c = p ? *p : 0;
	return *this;
}


StringTokenizer&	StringTokenizer::operator>> (short& i)
{
	const char*	p = get_next();
	i = p ? short(atoi(p)) : 0;
	return *this;
}


StringTokenizer&	StringTokenizer::operator>> (unsigned short& i)
{
	const char*	p = get_next();
	i = p ? (unsigned short)(atoi(p)) : 0;
	return *this;
}


StringTokenizer&	StringTokenizer::operator>> (int& i)
{
	const char*	p = get_next();
	i = p ? atoi(p) : 0;
	return *this;
}


StringTokenizer&	StringTokenizer::operator>> (unsigned int& i)
{
	const char*	p = get_next();
	i = p ? (unsigned int)(atoi(p)) : 0;
	return *this;
}


StringTokenizer&	StringTokenizer::operator>> (long& i)
{
	const char*	p = get_next();
	i = p ? atol(p) : 0;
	return *this;
}


StringTokenizer&	StringTokenizer::operator>> (unsigned long& i)
{
	const char*	p = get_next();
	i = p ? strtoul(p, 0, 10) : 0;
	return *this;
}


StringTokenizer&	StringTokenizer::operator>> (float& i)
{
	const char*	p = get_next();
	i = p ? strtof(p, 0) : 0.0;
	return *this;
}


StringTokenizer&	StringTokenizer::operator>> (double& i)
{
	const char*	p = get_next();
	i = p ? atof(p) : 0;
	return *this;
}


StringTokenizer&	StringTokenizer::operator>> (string& s)
{
	if (const char* p = get_next())
		s = p;
	else
		s.clear();

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


