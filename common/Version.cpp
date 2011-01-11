/*
 * common/Version.cpp
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

#include "Version.hpp"
#include "StringTokenizer.hpp"
#include <ostream>

using namespace LM;

void	Version::init (int M, int m, int p)
{
	major = M;
	minor = m;
	patch = p;
}

void	Version::init (const char* str)
{
	StringTokenizer(str, '.') >> major >> minor >> patch;
}

void	Version::clear ()
{
	major = minor = patch = 0;
}

bool	Version::operator< (Version other) const
{
	if (major == other.major) {
		if (minor == other.minor) {
			return patch < other.patch;
		} else {
			return minor < other.minor;
		}
	} else {
		return major < other.major;
	}
}

bool	Version::operator<= (Version other) const
{
	return *this < other || *this == other;
}

bool	Version::operator>= (Version other) const
{
	return *this > other || *this == other;
}

bool	Version::operator> (Version other) const
{
	if (major == other.major) {
		if (minor == other.minor) {
			return patch > other.patch;
		} else {
			return minor > other.minor;
		}
	} else {
		return major > other.major;
	}
}

bool	Version::operator== (Version other) const
{
	return major == other.major && minor == other.minor && patch == other.patch;
}

bool	Version::operator!= (Version other) const
{
	return !(*this == other);
}


std::ostream&	LM::operator<< (std::ostream& out, const Version& version)
{
	return out << version.major << '.' << version.minor << '.' << version.patch;
}

StringTokenizer&	LM::operator>> (StringTokenizer& tokenize, Version& version)
{
	if (const char* str = tokenize.get_next()) {
		version.init(str);
	} else {
		version.clear();
	}
	return tokenize;
}

