/*
 * common/misc.hpp
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

#ifndef LM_COMMON_MISC_H
#define LM_COMMON_MISC_H

#include <string>
#include <list>
#include <ctype.h>

// Signals to the program that it should clean up and exit; implemented elsewhere
extern "C" void clean_exit();

namespace LM {
	// Return the username of the current user.
	std::string		get_username();

	// Represent a color (with red, green, blue, and alpha components)
	struct Color {
		Color();
		Color(double r, double g, double b, double a = 1.0);
		double r;
		double g;
		double b;
		double a;

		static const Color WHITE;
		static const Color BLACK;
	};

	// remove all leading and trailing spaces of given string
	void		strip_leading_trailing_spaces(std::string& str);

	// condense all whitespace in the string to a single space character
	// (leading and trailing whitespace is removed)
	void		condense_whitespace(std::string& str, int (*my_isspace)(int) =isspace, char replacement =' ');

	inline int	istab(int c) { return c == '\t'; }

	// Sanitize/canonicalize a player name
	// Does the following things:
	//  Remove all leading and trailing whitespace
	//  Collapse multiple sequences of whitespace into one space
	//  Remove all control characters
	//  Limit the name to MAX_NAME_LENGTH characters (as defined in network.hpp)
	void		sanitize_player_name(std::string& str);

	// Daemonize the program (not on Windows)
	void		daemonize();

	// Drop privileges to the given user/group (not on Windows)
	// Either username or groupname may be NULL
	void		drop_privileges(const char* username, const char* groupname);

	// Determine whether the program has terminal output (always returns true on Windows)
	bool		has_terminal_output();

	// Scan the given directory and populate the given list with the name of every file and directory in the given directory
	bool		scan_directory(std::list<std::string>& filenames, const char* directory);
}

#endif
