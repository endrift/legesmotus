/*
 * common/misc.cpp
 *
 * This file is part of Leges Motus, a networked, 2D shooter set in zero gravity.
 * 
 * Copyright 2009-2010 Andrew Ayer, Nathan Partlan, Jeffrey Pfau
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

#include "misc.hpp"
#include "network.hpp"
#include "Exception.hpp"
#include "Version.hpp"
#include <string>
#include <cstring>
#include <cctype>
#include <sstream>

// See .hpp file for extensive comments.

using namespace LM;
using namespace std;

const Version LM::COMPAT_VERSION(0, 5, 0);

#ifdef __WIN32

#include <Windows.h>
#include <stdio.h>
#include <io.h>

string	LM::get_username() {
	char username[64];
	DWORD unlen = sizeof(username);
	if (GetUserName(username,&unlen)) {
		return string(username);
	} else {
		return "Bill";
	}
}

void	LM::daemonize() {
	throw Exception("Sorry, daemonization not supported on Windows.");
}

void	LM::drop_privileges(const char* username, const char* groupname) {
	throw Exception("Sorry, privilege dropping is not supported on Windows.");
}

bool	LM::has_terminal_output() {
	return _isatty(_fileno(stdout));
}

bool	LM::scan_directory(list<string>& filenames, const char* directory) {
	string			full_directory(directory);
	if (full_directory[full_directory.size() - 1] != '\\') {
		full_directory += "\\";
	}
	full_directory += "*";

	WIN32_FIND_DATA		file_data;
	HANDLE			find_handle = FindFirstFile(full_directory.c_str(), &file_data);
	if (find_handle == INVALID_HANDLE_VALUE) {
		return GetLastError() == ERROR_FILE_NOT_FOUND;
	}

	do {
		const char*	filename = file_data.cFileName;
		if (strcmp(filename, ".") != 0 && strcmp(filename, "..") != 0) {
			filenames.push_back(filename);
		}
	} while (FindNextFile(find_handle, &file_data));

	bool			success = GetLastError() == ERROR_NO_MORE_FILES;
	FindClose(find_handle);
	return success;
}


#else

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pwd.h>
#include <grp.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <dirent.h>

string	LM::get_username() {
	if (struct passwd* pw = getpwuid(getuid())) {
		return pw->pw_name;
	} else {
		return "Tux";
	}
}

void	LM::daemonize() {
	// Fork, and exit the parent process
	pid_t	pid = fork();
	if (pid < 0) {
		throw Exception(strerror(errno));
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

void	LM::drop_privileges(const char* username, const char* groupname) {
	if (!username && !groupname) {
		return;
	}

	struct passwd*		usr = NULL;
	struct group*		grp = NULL;
	if (username) {
		errno = 0;
		if (!(usr = getpwnam(username))) {
			ostringstream	errmsg;
			errmsg << username << ": " << (errno ? strerror(errno) : "No such user");
			throw Exception(errmsg.str().c_str());
		}
	}

	if (groupname) {
		errno = 0;
		if (!(grp = getgrnam(groupname))) {
			ostringstream	errmsg;
			errmsg << groupname << ": " << (errno ? strerror(errno) : "No such group");
			throw Exception(errmsg.str().c_str());
		}
	}

	// If no group is specified, but a user is specified, drop to the primary GID of that user
	if (setgid(grp ? grp->gr_gid : usr->pw_gid) < 0) {
		ostringstream	errmsg;
		errmsg << "Failed to drop privileges: " << strerror(errno);
		throw Exception(errmsg.str().c_str());
	}

	if (usr) {
		initgroups(usr->pw_name, usr->pw_gid);
		if (setuid(usr->pw_uid) < 0) {
			ostringstream	errmsg;
			errmsg << "Failed to drop privileges: " << strerror(errno);
			throw Exception(errmsg.str().c_str());
		}
	}
}

bool	LM::has_terminal_output() {
	return isatty(1);
}

bool	LM::scan_directory(list<string>& filenames, const char* directory) {
	struct dirent**	files;
	int		nfiles = scandir(directory, &files, NULL, NULL);
	if (nfiles < 0) {
		return false;
	}

	for (int i = 0; i < nfiles; ++i) {
		const char*	filename = files[i]->d_name;
		if (strcmp(filename, ".") != 0 && strcmp(filename, "..") != 0) {
			filenames.push_back(filename);
		}
		free(files[i]);
	}
	free(files);

	return true;
}

#endif

Color::Color() {
	r = 0;
	g = 0;
	b = 0;
	a = 1;
}

Color::Color(float r, float g, float b, float a) {
	this->r = r;
	this->g = g;
	this->b = b;
	this->a = a;
}

Color::Color(int r, int g, int b, int a) {
	this->r = r / 255.0f;
	this->g = g / 255.0f;
	this->b = b / 255.0f;
	this->a = a / 255.0f;
}

const Color Color::WHITE(1.0f, 1.0f, 1.0f);
const Color Color::BLACK(0.0f, 0.0f, 0.0f);

void	LM::strip_leading_trailing_spaces(string& str) {
	string::size_type startpos = str.find_first_not_of(" \t");
	string::size_type endpos = str.find_last_not_of(" \t");
	if (startpos != string::npos && endpos != string::npos) {
		str = str.substr(startpos, endpos - startpos + 1);
	} else {
		str.clear();
	}
}

void	LM::condense_whitespace(string& str, int (*my_isspace)(int), char replacement) {
	const char*	p = str.c_str();

	// Skip leading whitespace
	while (my_isspace(*p)) {
		++p;
	}

	string		new_str;
	bool		is_in_whitespace = false;
	while (*p) {
		char	c = *p++;
		if (my_isspace(c)) {
			// Whitespace - skip it for now
			is_in_whitespace = true;
		} else {
			if (is_in_whitespace) {
				// End of an all-whitespace region.  Lay down a single space.
				is_in_whitespace = false;
				new_str += replacement;
			}
			new_str += c;
		}
	}

	str.swap(new_str);
}

void	LM::sanitize_player_name(string& name) {
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

