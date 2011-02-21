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

using namespace LM;
using namespace std;

void FuzzyCategory::Bin::clear() {
	start = 0.0f;
	end = 0.0f;
	grade_width = 0.0f;
}

int FuzzyCategory::add_bin(float start, float end, float grade_width) {
	return add_bin((Bin){ start, end, grade_width });
}

int FuzzyCategory::add_bin(const Bin& bin) {
	m_bins.push_back(bin);
	return m_bins.size() - 1;
}

void FuzzyCategory::apply(float value, vector<float>* results) const {
	results->clear();

	for (vector<Bin>::const_iterator iter = m_bins.begin(); iter != m_bins.end(); ++iter) {
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
		results->push_back(result);
	}
}
