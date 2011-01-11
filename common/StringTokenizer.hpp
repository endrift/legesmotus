/*
 * common/StringTokenizer.hpp
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

#ifndef LM_COMMON_STRINGTOKENIZER_HPP
#define LM_COMMON_STRINGTOKENIZER_HPP

#include <string>
#include <stdint.h>
#include <iosfwd>
#include <algorithm>

namespace LM {
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
	public:
		enum { max_delimiters = 4 };
	private:
		char		m_delimiters[max_delimiters];	// The characters we're splitting on
		char*		m_buffer;	// A copy of the data we're splitting
		char*		m_next_token;	// A pointer into m_buffer to the start of the next token to extract
		size_t		m_tokens_left;	// Number of tokens left to extract
	
		bool		is_delimiter(char c) const;

		void		deinit();
	
	public:
		StringTokenizer();
		StringTokenizer(const std::string& str, char delimiter) { m_buffer = NULL; init(str.c_str(), delimiter); }
		StringTokenizer(const std::string& str, char delimiter, size_t max_tokens) { m_buffer = NULL; init(str.c_str(), delimiter, max_tokens); }
		StringTokenizer(const char* str, char delimiter) { m_buffer = NULL; init(str, delimiter); }
		StringTokenizer(const char* str, char delimiter, size_t max_tokens) { m_buffer = NULL; init(str, delimiter, max_tokens); }

		StringTokenizer(const std::string& str, const char* delimiters, bool condense =false) { m_buffer = NULL; init(str.c_str(), delimiters, condense); }
		StringTokenizer(const std::string& str, const char* delimiters, bool condense, size_t max_tokens) { m_buffer = NULL; init(str.c_str(), delimiters, condense, max_tokens); }
		StringTokenizer(const char* str, const char* delimiters, bool condense =false) { m_buffer = NULL; init(str, delimiters, condense); }
		StringTokenizer(const char* str, const char* delimiters, bool condense, size_t max_tokens) { m_buffer = NULL; init(str, delimiters, condense, max_tokens); }

		StringTokenizer(const StringTokenizer& other);

		~StringTokenizer();

		// Initialize
		void			init(const char* str, char delimiter);
		void			init(const char* str, char delimiter, size_t max_tokens);
		void			init(const char* str, const char* delimiters, bool condense =false);
		void			init(const char* str, const char* delimiters, bool condense, size_t max_tokens);
		void			init_from_raw_data(const char* str, size_t len, bool condense =false); // Initialize from the first len characters of str
		void			set_delimiter(char);
		void			set_delimiters(const char*);

		StringTokenizer& operator=(const StringTokenizer&);
	
		// Get the next token
		const char*		get_next ();
	
		// Discard (i.e. ignore) the next field
		void			discard_next() { get_next(); }
	
		// Get the remaining tokens as one string
		const char*		get_rest () const;
	
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
	
		StringTokenizer&	operator>> (long long&);
		StringTokenizer&	operator>> (unsigned long long&);
	
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
	
		void		swap(StringTokenizer& other);
	};

}

namespace std {
	template<> inline void swap (LM::StringTokenizer& x, LM::StringTokenizer& y) { x.swap(y); }
}

#endif
