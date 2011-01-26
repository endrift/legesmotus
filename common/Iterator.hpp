/*
 * common/Iterator.hpp
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

#ifndef LM_COMMON_ITERATOR_HPP
#define LM_COMMON_ITERATOR_HPP

#include <map>
#include <vector>

namespace LM {
	template <typename T>
	class Iterator {
	public:
		class OpaqueIterator {
		public:
			virtual ~OpaqueIterator() {}
			virtual bool has_more() const = 0;
			virtual T next() = 0;
			virtual OpaqueIterator* clone() = 0;
		};

		template <typename I>
		class StdIterator : public OpaqueIterator {
		private:
			I* m_data;
			typename I::iterator m_iter;
		public:
			StdIterator(I* data) {
				m_data = data;
				m_iter = data->begin();
			}

			virtual bool has_more() const {
				return m_iter != m_data->end();
			}

			virtual T next() {
				return *(m_iter++);
			}

			virtual StdIterator<I>* clone() {
				return new StdIterator(*this);
			}
		};

	private:
		OpaqueIterator* m_internal;
	public:
		Iterator(OpaqueIterator* real_iterator) {
			m_internal = real_iterator;
		}

		Iterator(const Iterator<T>& other) {
			m_internal = other.m_internal->clone();
		}

		~Iterator() {
			delete m_internal;
		}

		bool has_more() const {
			return m_internal->has_more();
		}

		T next() {
			return m_internal->next();
		}
	};


	template <typename M, typename N>
	class StdMapIterator : public Iterator<std::pair<M, N> >::template StdIterator<std::map<M, N> > {
	public:
		StdMapIterator(std::map<M, N>* map) : Iterator<std::pair<M, N> >::template StdIterator<std::map<M, N> >(map) {}
	};

	template <typename M, typename N>
	class StdMultiMapIterator : public Iterator<std::pair<M, N> >::template StdIterator<std::multimap<M, N> > {
	public:
		StdMultiMapIterator(std::multimap<M, N>* map) : Iterator<std::pair<M, N> >::template StdIterator<std::multimap<M, N> >(map) {}
	};

	template <typename T>
	class StdVectorIterator : public Iterator<T>::template StdIterator<std::vector<T> > {
	public:
		StdVectorIterator(std::vector<T>* vector) : Iterator<T>::template StdIterator<std::vector<T> >(vector) {}
	};
}

#endif
