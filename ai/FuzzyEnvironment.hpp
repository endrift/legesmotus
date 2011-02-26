/*
 * ai/FuzzyEnvironment.hpp
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

#ifndef LM_AI_FUZZYENVIRONMENT_HPP
#define LM_AI_FUZZYENVIRONMENT_HPP

#include <map>
#include "common/Iterator.hpp"

namespace LM {
	class FuzzyEnvironment {
	public:
		class Subenv {
		private:
			FuzzyEnvironment* m_e;
			int m_cat;

		public:
			Subenv(FuzzyEnvironment* parent, int category);

			void set(long id, int bin, float value);
			float get(long id, int bin) const;

			ConstIterator<std::pair<long, float> > get_input() const;

			void clear();
		};

	private:
		std::map<int, std::map<long, std::map<int, float> > > m_env;
		std::map<int, std::map<long, float> > m_input;

	public:
		void set(int cat, int bin, float value);
		void set(int cat, long id, int bin, float value);
		float get(int cat, int bin) const;
		float get(int cat, long id, int bin) const;

		void set_input(int cat, float value);
		void set_input(int cat, const std::map<long, float>& input);
		ConstIterator<std::pair<long, float> > get_input(int cat) const;

		void clear(int cat);

		Subenv subset(int cat);
	};
}

#endif
