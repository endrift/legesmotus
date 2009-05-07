/*
 * common/misc.cpp
 *
 * Copyright 2009 - Nathan Partlan, Andrew Ayer, Daniel Schneider, and Jeffrey Pfau
 * 
 */

#include "misc.hpp"

#ifdef __WIN32

#include <Windows.h>

std::string	get_username() {
	char username[64];
	size_t unlen = sizeof(username);
	if (GetUserName(username,&unlen)) {
		return std::string(username);
	} else {
		return "Bill";
	}
}

#else

#include <sys/types.h>
#include <pwd.h>
#include <unistd.h>

std::string	get_username() {
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
