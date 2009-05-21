/*
 * common/misc.hpp
 *
 * Copyright 2009 - Nathan Partlan, Andrew Ayer, Daniel Schneider, and Jeffrey Pfau
 * 
 */

#ifndef LM_COMMON_MISC_H
#define LM_COMMON_MISC_H

#include <string>


// Return the username of the current user.
std::string		get_username();

// Signals to the program that it should clean up and exit; implemented elsewhere
extern "C" void clean_exit();

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

// remove all trailing spaces of given string
void		strip_trailing_spaces(std::string& str);

// Sanitize/canonicalize a player name
// Does the following things:
//  Remove all leading and trailing whitespace
//  Collapse multiple sequences of whitespace into one space
//  Remove all control characters
//  Limit the name to MAX_NAME_LENGTH characters (as defined in network.hpp)
void		sanitize_player_name(std::string& str);

// Daemonize the program (not on Windows)
void		daemonize();

#endif
