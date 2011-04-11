/*
 * common/FiniteStateMachine.hpp
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

#ifndef LM_COMMON_FINITESTATEMACHINE_HPP
#define LM_COMMON_FINITESTATEMACHINE_HPP

#include <map>

namespace LM {
	template <typename K, typename S>
	class FiniteStateMachine {
	private:
		const K& m_current_state;
		std::map<K, S*> m_statelist;

	protected:
		virtual const K& get_transition() = 0;

	public:
		FiniteStateMachine(const K& start_state);
		virtual ~FiniteStateMachine() { }

		void set_state_data(const K& id, S* state);
		const S* get_state_data(const K& id) const;
		S* get_state_data(const K& id);

		const K& get_current_state() const;
		const S* get_current_state_data() const;
		S* get_current_state_data();

		const K& transition();
	};

	template <typename K, typename S>
	FiniteStateMachine<K, S>::FiniteStateMachine(const K& start_state) {
		m_current_state = start_state;
	}

	template <typename K, typename S>
	void FiniteStateMachine<K, S>::set_state_data(const K& id, S* state) {
		m_statelist[id] = state;
	}

	template <typename K, typename S>
	const S* FiniteStateMachine<K, S>::get_state_data(const K& id) const {
		ASSERT(m_statelist->find(id) != m_statelist->end());
		return m_statelist->find(id)->second;
	}

	template <typename K, typename S>
	S* FiniteStateMachine<K, S>::get_state_data(const K& id) {
		ASSERT(m_statelist->find(id) != m_statelist->end());
		return m_statelist->find(id)->second;
	}

	template <typename K, typename S>
	const K& FiniteStateMachine<K, S>::get_current_state() const {
		return m_current_state;
	}

	template <typename K, typename S>
	const S* FiniteStateMachine<K, S>::get_current_state_data() const {
		return get_state_data(m_current_state);
	}

	template <typename K, typename S>
	S* FiniteStateMachine<K, S>::get_current_state_data() {
		return get_state_data(m_current_state);
	}

	template <typename K, typename S>
	const K& FiniteStateMachine<K, S>::transition() {
		m_current_state = get_transition();
		return m_current_state;
	}
}

#endif
