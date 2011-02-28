/*
 * ai/FuzzyCategory.cpp
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

#include "FuzzyCategory.hpp"
#include "common/misc.hpp"

#include <limits>

using namespace LM;
using namespace std;

void FuzzyCategory::Bin::clear() {
	start = numeric_limits<float>::min();
	end = numeric_limits<float>::max();
	grade_width = 0.0f;
}

int FuzzyCategory::add_bin(const string& id, float start, float end, float grade_width) {
	return add_bin(id, (Bin){ start, end, grade_width });
}

int FuzzyCategory::add_bin(const string& id, const Bin& bin) {
	if (m_ids.find(id) != m_ids.end()) {
		int int_id = m_ids[id];
		m_bins[int_id] = bin;
		return int_id;
	} else {
		int int_id = m_bins.size();
		m_ids[id] = int_id;
		m_bins.push_back(bin);
		return int_id;
	}
}

int FuzzyCategory::get_bin_id(const string& name) const {
	ASSERT(m_ids.find(name) != m_ids.end());
	return m_ids.find(name)->second;
}

void FuzzyCategory::apply(FuzzyEnvironment::Subenv results) const {
	int bid = 0;
	for (vector<Bin>::const_iterator iter = m_bins.begin(); iter != m_bins.end(); ++iter, ++bid) {
		ConstIterator<std::pair<long, float> > input = results.get_input();
		while (input.has_more()) {
			pair<long, float> in_pair = input.next();
			float value = in_pair.second;
			float result = 0.0f;
			const Bin& b = *iter;
			if (value >= b.start) {
				if (value <= b.end) {
					result = 1.0f;
				} else if (value <= b.end + b.grade_width) {
					result = (-value + b.end + b.grade_width)/b.grade_width;
				}
			} else if (value >= b.start - b.grade_width) {
				result = (value - b.start + b.grade_width)/b.grade_width;
			}
			results.set(in_pair.first, bid, result);
		}
	}
}
