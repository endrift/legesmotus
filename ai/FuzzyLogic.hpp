/*
 * ai/FuzzyLogic.hpp
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

#ifndef LM_AI_FUZZYLOGIC_HPP
#define LM_AI_FUZZYLOGIC_HPP

#include <map>

namespace LM {
	class FuzzyLogic {
	public:
		class Rule {
		public:
			virtual ~Rule() {}
			// Takes a map of pair<category, bin> to value and returns a value for that category
			virtual float apply(const std::map<int, std::map<int, float> >& values) const = 0;
		};

		class Terminal : public Rule {
		private:
			int m_cat;
			int m_id;

		public:
			Terminal(int cat, int id);
			virtual float apply(const std::map<int, std::map<int, float> >& values) const;
		};

		class And : public Rule {
		private:
			const Rule* m_lhs;
			const Rule* m_rhs;

		public:
			And(const Rule* lhs, const Rule* rhs);
			virtual ~And();
			virtual float apply(const std::map<int, std::map<int, float> >& values) const;
		};

		class Or : public Rule {
		private:
			const Rule* m_lhs;
			const Rule* m_rhs;

		public:
			Or(const Rule* lhs, const Rule* rhs);
			virtual ~Or();
			virtual float apply(const std::map<int, std::map<int, float> >& values) const;
		};

		class Not : public Rule {
		private:
			const Rule* m_op;

		public:
			Not(const Rule* op);
			virtual ~Not();
			virtual float apply(const std::map<int, std::map<int, float> >& values) const;
		};

	private:
		struct Bin {
			float start;
			float end;
			float height;
			float grade_width;
		};

		std::map<int, std::map<int, Bin> > m_bins;

	public:
		// Add bin id to category cat
		void add_bin(int cat, int id, float height, float start, float end, float grade_width);
		// Apply values in the categories to the appropriate bins and return the results for each bin category and bin
		void apply(std::map<int, float> values, std::map<int, std::map<int, float> >* results) const;
	};
}

#endif
