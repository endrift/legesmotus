/*
 * gui/pubsub.cpp
 *
 * This file is part of Leges Motus, a networked, 2D shooter set in zero gravity.
 * 
 * Copyright 2009-2010 Andrew Ayer, Nathan Partlan, Jeffrey Pfau
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

#include "pubsub.hpp"

using namespace LM;
using namespace std;

ReqSub::~ReqSub() {
	for (map<long, std::pair<long, Reprocessor*> >::iterator iter = m_reproc.begin(); iter != m_reproc.end(); iter++) {
		delete iter->second.second;
	}
}

void ReqSub::rebind(PubSub old_type, PubSub new_type, Reprocessor* proc) {
	if (m_reproc.find(old_type.id) != m_reproc.end()) {
		delete m_reproc[old_type.id].second;
	}
	m_reproc[old_type.id] = make_pair(new_type.id, proc);
}

void ReqSub::handle_pub(PubSub type, Publisher* pub, void* data) {
	if (m_reproc.find(type.id) != m_reproc.end()) {
		pair<long, Reprocessor*> p = m_reproc[type.id];
		Reprocessor* re = p.second;
		data = (*re)(type.p, data);
		type.id = p.first;
		
		handle_req(type, data);

		re->clean(data);
	}
}

void Publisher::raise(PubSub type) {
	pair<Subscriber*, void*> l = m_subs[type.id];
	l.first->handle_pub(type, this, l.second);
}

void Publisher::set_subscriber(PubSub type, Subscriber* sub, void* data) {
	m_subs[type.id] = make_pair(sub, data);
}
