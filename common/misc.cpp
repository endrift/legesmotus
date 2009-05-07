/*
 * common/misc.cpp
 *
 * Copyright 2009 - Nathan Partlan, Andrew Ayer, Daniel Schneider, and Jeffrey Pfau
 * 
 */

#include "misc.hpp"

// See .hpp file for extensive comments.

using namespace std;

#ifdef __WIN32

#include <Windows.h>

string	get_username() {
	char username[64];
	size_t unlen = sizeof(username);
	if (GetUserName(username,&unlen)) {
		return string(username);
	} else {
		return "Bill";
	}
}

#else

#include <sys/types.h>
#include <pwd.h>
#include <unistd.h>

string	get_username() {
	if (struct passwd* pw = getpwuid(getuid())) {
		return pw->pw_name;
	} else {
		return "Tux";
	}
}

#endif

Color::Color() {
	r = 0;
	g = 0;
	b = 0;
	a = 1;
}

Color::Color(double r, double g, double b, double a) {
	this->r = r;
	this->g = g;
	this->b = b;
	this->a = a;
}

void	strip_trailing_spaces(string& str) {
	string::size_type endpos = str.find_last_not_of("\n\t");
	str = str.substr(0, endpos);
}

