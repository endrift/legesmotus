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

/*
 * This string tokenizer allows convenient tokenization of a string based upon a single-character delimiter.
 * Just construct a string tokenizer with a string to tokenize and a delimiter, and use the >> operator to read each token, one-by-one.
 * Optionally, a maximum number of tokens can be specified, after which all remaining data gets treated as one token.
 *
 * Example of use:
 * 	StringTokenizer	tok("gun/rotation/5.2", '/');
 *
 * 	std::string	type;
 * 	std::string	subtype;
 * 	double		value;
 * 	tok >> type >> subtype >> value;
 *
 *	// type is now "gun"
 *	// subtype is now "rotation"
 *	// value is now 5.2
 */
class StringTokenizer {
private:
	char		m_delimiter;	// The character we're splitting on
	char*		m_buffer;	// A copy of the data we're splitting
	char*		m_next_token;	// A pointer into m_buffer to the start of the next token to extract
	size_t		m_tokens_left;	// Number of tokens left to extract

	// disallow copy and assignment
	StringTokenizer(const StringTokenizer&) { }
	StringTokenizer& operator=(const StringTokenizer&) { return *this; }

public:
	StringTokenizer(const std::string& str, char delimiter) { m_buffer = NULL; init(str.c_str(), delimiter); }
	StringTokenizer(const std::string& str, char delimiter, size_t max_tokens) { m_buffer = NULL; init(str.c_str(), delimiter, max_tokens); }
	StringTokenizer(const char* str, char delimiter) { m_buffer = NULL; init(str, delimiter); }
	StringTokenizer(const char* str, char delimiter, size_t max_tokens) { m_buffer = NULL; init(str, delimiter, max_tokens); }
	~StringTokenizer();

	// Initialize
	void			init(const char* str, char delimiter);
	void			init(const char* str, char delimiter, size_t max_tokens);

	// Get the next token
	const char*		get_next ();

	// Discard (i.e. ignore) the next field
	void			discard_next() { get_next(); }

	// Get the remaining tokens as one string
	const char*		get_rest ();

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

	// Are there any more tokens available to be read?
	bool		has_more() const { return m_next_token != NULL; }
	bool		has_next() const { return m_next_token != NULL; }

	// To test whether there are any tokens left for processing
	bool		operator! () const { return !has_more(); }
			operator const void* () const { return has_more() ? this : NULL; }

};

#endif
