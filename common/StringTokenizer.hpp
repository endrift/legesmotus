/*
 * common/StringTokenizer.hpp
 *
 * Copyright 2009 - Nathan Partlan, Andrew Ayer, Daniel Schneider, and Jeffrey Pfau
 * 
 */

#ifndef LM_COMMON_STRINGTOKENIZER_HPP
#define LM_COMMON_STRINGTOKENIZER_HPP

#include <string>
#include <stdint.h>
#include <iosfwd>

class Point;

class StringTokenizer {
private:
	char		m_delimiter;
	char*		m_buffer;
	char*		m_next_token;

	const char*	get_next ();	// Advance to the next token and return the old one.

public:
	// Construct a packet reader from the given raw packet data
	explicit StringTokenizer(const char* str, char delimiter);
	~StringTokenizer();

	// Discard (i.e. ignore) the next field
	void		discard_next() { get_next(); }

	// Get the remaining tokens as one string
	const char*	get_rest ();

	// The following functions read the next token into the variable of the given type:
	StringTokenizer&	operator>> (bool&);
	StringTokenizer&	operator>> (char&);
	StringTokenizer&	operator>> (unsigned char&);

	StringTokenizer&	operator>> (short&);
	StringTokenizer&	operator>> (unsigned short&);

	StringTokenizer&	operator>> (int&);
	StringTokenizer&	operator>> (unsigned int&);

	StringTokenizer&	operator>> (long&);
	StringTokenizer&	operator>> (unsigned long&);

	StringTokenizer&	operator>> (float&);
	StringTokenizer&	operator>> (double&);

	StringTokenizer&	operator>> (Point&);

	StringTokenizer&	operator>> (std::string&);

	bool		has_more() const { return m_next_token != NULL; }

	// To test whether there are any fields left for processing
	bool		operator! () const { return !has_more(); }
			operator const void* () const { return has_more() ? this : NULL; }

};

#endif
