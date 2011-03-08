/*
 * ai/SparseIntersectMap.hpp
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

#ifndef LM_AI_SPARSEINTERSECTMAP_HPP
#define LM_AI_SPARSEINTERSECTMAP_HPP

#include "common/Iterator.hpp"

namespace LM {
	class SparseIntersectMap {
		class ConstMapIterator;

		friend class SparseIntersectMap::ConstMapIterator;

	public:
		struct Intersect {
			int x;
			int y;
			float dist;
		};

	private:
		struct Element {
			int x;
			int y;
			int t;
			Intersect i;
		};

		struct Bucket {
			int psize;
			int nsize;
			Element* elts;
		};

		Bucket* m_buckets;
		int m_nbuckets;
		int m_grain;
		int m_count;

		int make_hash(int x, int y, int theta) const;

		int grain_x(float x) const;
		int grain_y(float y) const;
		int grain_theta(float theta) const;

	public:
		SparseIntersectMap(int granularity, int est_elts);
		SparseIntersectMap(std::istream* f);
		~SparseIntersectMap();

		void set(float x, float y, float theta, const Intersect& isect);
		bool get(float x, float y, float theta, Intersect* isect) const;

		int count() const;

		ConstIterator<const Intersect&> iterate() const;
		void write(std::ostream* f) const;

		float get_granularity_x() const;
		float get_granularity_y() const;
		float get_granularity_theta() const;
	};
}

#endif
