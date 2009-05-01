/*
 * common/misc.cpp
 *
 * Copyright 2009 - Nathan Partlan, Andrew Ayer, Daniel Schneider, and Jeffrey Pfau
 * 
 */

#include "misc.hpp"

#ifdef __WIN32

std::string	get_username() {
	return "Bill";
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
