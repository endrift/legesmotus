/*
 * common/misc.cpp
 *
 * Copyright 2009 - Nathan Partlan, Andrew Ayer, Daniel Schneider, and Jeffrey Pfau
 * 
 */

#include "misc.hpp"
#include "network.hpp"
#include "LMException.hpp"
#include <string>
#include <cstring>
#include <cctype>

// See .hpp file for extensive comments.

using namespace std;

#ifdef __WIN32

#include <Windows.h>

string	get_username() {
	char username[64];
	DWORD unlen = sizeof(username);
	if (GetUserName(username,&unlen)) {
		return string(username);
	} else {
		return "Bill";
	}
}

void	daemonize() {
	throw LMException("Sorry, daemonization not supported on Windows.");
}

#else

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pwd.h>
#include <unistd.h>
#include <errno.h>

string	get_username() {
	if (struct passwd* pw = getpwuid(getuid())) {
		return pw->pw_name;
	} else {
		return "Tux";
	}
}

void	daemonize() {
	// Fork, and exit the parent process
	pid_t	pid = fork();
	if (pid < 0) {
		throw LMException(strerror(errno));
	} else if (pid > 0) {
		// Parent process
		exit(0);
	}

	// Dupe stdin, stdout, and stderr to /dev/null
	if (isatty(0)) {
		int fd = open("/dev/null", O_RDONLY);
		if (fd < 0) {
			close(0);
		} else {
			dup2(fd, 0);
			close(fd);
		}
	}
	if (isatty(1)) {
		int fd = open("/dev/null", O_WRONLY);
		if (fd < 0) {
			close(1);
		} else {
			dup2(fd, 1);
			close(fd);
		}
	}
	if (isatty(2)) {
		int fd = open("/dev/null", O_WRONLY);
		if (fd < 0) {
			close(2);
		} else {
			dup2(fd, 2);
			close(fd);
		}
	}

	// Create a new session
	setsid();
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

const Color Color::WHITE(1.0, 1.0, 1.0);
const Color Color::BLACK(0.0, 0.0, 0.0);

void	strip_trailing_spaces(string& str) {
	string::size_type endpos = str.find_last_not_of(" \t");
	if (endpos != string::npos) {
		str = str.substr(0, endpos + 1);
	}
}

void	sanitize_player_name(string& name) {
	const char*	p = name.c_str();

	// Skip leading whitespace
	while (isspace(*p)) {
		++p;
	}

	string		new_name;
	bool		is_in_whitespace = false;
	while (*p && new_name.size() < MAX_NAME_LENGTH) {
		char	c = *p++;
		if (isspace(c)) {
			// Whitespace - skip it for now
			is_in_whitespace = true;
		} else {
			if (is_in_whitespace) {
				// End of an all-whitespace region.  Lay down a single space.
				is_in_whitespace = false;
				new_name += ' ';
			}
			if (!iscntrl(c)) {
				// Only allow non-control characters.  Boy, the Sunlab is a tough crowd.
				new_name += c;
			}
		}
	}

	name.swap(new_name);
}

