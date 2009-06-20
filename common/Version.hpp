/*
 * common/Version.hpp
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

#ifndef LM_COMMON_VERSION_HPP
#define LM_COMMON_VERSION_HPP

#include <iosfwd>

namespace LM {
	class StringTokenizer;
	
	class Version {
	public:
		int	major;
		int	minor;
		int	patch;
	
		Version () { clear(); }
		Version (int M, int m, int p) { init(M, m, p); }
		explicit Version (const char* str) { init(str); }
	
		void	clear ();
		void	init (int, int, int);
		void	init (const char* str);
	
		bool	operator< (Version other) const;
		bool	operator<= (Version other) const;
		bool	operator>= (Version other) const;
		bool	operator> (Version other) const;
		bool	operator== (Version other) const;
		bool	operator!= (Version other) const;
	};
	
	std::ostream&		operator<< (std::ostream&, const Version&);
	StringTokenizer&	operator>> (StringTokenizer&, Version&);
	
}

#endif
