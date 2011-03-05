/*
 * ai/SparseIntersectMap.cpp
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

#include "SparseIntersectMap.hpp"
#include "common/misc.hpp"

#include <cstring>

using namespace LM;
using namespace std;

class SparseIntersectMap::ConstMapIterator : public ConstIterator<const SparseIntersectMap::Intersect&>::OpaqueIterator {
private:
	const SparseIntersectMap* m_map;
	int m_bucket;
	int m_i;

	void seek_next();

public:
	ConstMapIterator(const SparseIntersectMap* map);

	virtual bool has_more() const;
	virtual const SparseIntersectMap::Intersect& next();
	virtual ConstMapIterator* clone() const;
};

SparseIntersectMap::ConstMapIterator::ConstMapIterator(const SparseIntersectMap* map) {
	m_map = map;
	m_bucket = 0;
	m_i = 0;
	seek_next();
}

void SparseIntersectMap::ConstMapIterator::seek_next() {
	ASSERT(m_bucket < m_map->m_nbuckets);

	if (++m_i < m_map->m_buckets[m_bucket].nsize) {
		return;
	}
	for (m_i = 0; m_bucket < m_map->m_nbuckets; ++m_bucket) {
		if (m_map->m_buckets[m_bucket].nsize) {
			return;
		}
	}
}

bool SparseIntersectMap::ConstMapIterator::has_more() const {
	return m_bucket >= m_map->m_nbuckets;
}

const SparseIntersectMap::Intersect& SparseIntersectMap::ConstMapIterator::next() {
	const Intersect& n = m_map->m_buckets[m_bucket].elts[m_i].i;
	seek_next();
	return n;
}

SparseIntersectMap::ConstMapIterator* SparseIntersectMap::ConstMapIterator::clone() const {
	return new ConstMapIterator(*this);
}

SparseIntersectMap::SparseIntersectMap(int granularity, int est_elts) {
	m_grain = granularity;
	m_nbuckets = est_elts >> 5;
	if (m_nbuckets < 16) {
		m_nbuckets = 16;
	}
	m_buckets = new Bucket[m_nbuckets];
	memset(m_buckets, 0, m_nbuckets * sizeof(Bucket));
}

SparseIntersectMap::~SparseIntersectMap() {
	for (int i = 0; i < m_nbuckets; ++i) {
		delete m_buckets[i].elts;
	}
	delete m_buckets;
}

int SparseIntersectMap::make_hash(int x, int y, int theta) const {
	int hash = x + y + theta;
	// Ensure hash is non-negative
	if (hash < 0) {
		hash = -1 - hash;
	}
	return hash % m_nbuckets;
}

int SparseIntersectMap::grain_x(float x) const {
	return int(x) >> m_grain;
}

int SparseIntersectMap::grain_y(float y) const {
	return int(y) >> m_grain;
}

int SparseIntersectMap::grain_theta(float theta) const {
	return int(theta / 360.0f * 8192.0f) >> m_grain;
}

void SparseIntersectMap::set(float x, float y, float theta, const Intersect& isect) {
	int gx = grain_x(x);
	int gy = grain_y(y);
	int gt = grain_theta(theta);
	Bucket& bucket = m_buckets[make_hash(gx, gy, gt)];

	ASSERT(bucket.psize >= bucket.nsize);

	if (bucket.psize == 0) {
		bucket.psize = 32;
		bucket.elts = new Element[bucket.psize];
		bucket.elts[0].x = gx;
		bucket.elts[0].y = gy;
		bucket.elts[0].t = gt;
		bucket.elts[0].i = isect;
		bucket.nsize = 1;
	}

	for (int i = 0; i < bucket.nsize; ++i) {
		Element& e = bucket.elts[i];
		if (e.x == gx && e.y == gy && e.t == gt) {
			e.i = isect;
			return;
		}
	}

	if (bucket.psize == bucket.nsize) {
		bucket.psize <<= 1;
		Element* nelts = new Element[bucket.psize];
		memcpy(nelts, bucket.elts, sizeof(Element)*bucket.nsize);
		delete bucket.elts;
		bucket.elts = nelts;
	}

	bucket.elts[bucket.nsize].x = gx;
	bucket.elts[bucket.nsize].y = gy;
	bucket.elts[bucket.nsize].t = gt;
	bucket.elts[bucket.nsize].i = isect;
	++bucket.nsize;
}

bool SparseIntersectMap::get(float x, float y, float theta, Intersect* isect) const {
	int gx = grain_x(x);
	int gy = grain_y(y);
	int gt = grain_theta(theta);
	const Bucket& bucket = m_buckets[make_hash(gx, gy, gt)];

	for (int i = 0; i < bucket.nsize; ++i) {
		const Element& e = bucket.elts[i];
		if (e.x == gx && e.y == gy && e.t == gt) {
			*isect = e.i;
			return true;
		}
	}

	return false;
}

ConstIterator<const SparseIntersectMap::Intersect&> SparseIntersectMap::iterate() const {
	return ConstIterator<const SparseIntersectMap::Intersect&>(new ConstMapIterator(this));
}

float SparseIntersectMap::get_granularity_x() const {
	return 1 << m_grain;
}

float SparseIntersectMap::get_granularity_y() const {
	return 1 << m_grain;
}

float SparseIntersectMap::get_granularity_theta() const {
	return (1 << m_grain) * 360.0f / 8192.0f;
}
