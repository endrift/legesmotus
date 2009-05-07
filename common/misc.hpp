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

// Represent a color (with red, green, blue, and alpha components)
struct Color {
	Color();
	Color(double r, double g, double b, double a = 1.0);
	double r;
	double g;
	double b;
	double a;
};

// remove all trailing spaces of given string
void		strip_trailing_spaces(std::string& str);

#endif
