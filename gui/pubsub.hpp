/*
 * gui/pubsub.hpp
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

#ifndef LM_GUI_PUBSUB_HPP
#define LM_GUI_PUBSUB_HPP

#include <map>

#define LM_DECLARE_PUBSUB(type) extern const char* PUBSUB_ ## type
#define LM_DEFINE_PUBSUB(type, signature) const char* PUBSUB_ ## type = "PUBSUB:" # signature
#define LM_ASSIGN_PUBSUB(l, type) ((l)->p = PUBSUB_ ## type)
#define LM_SWITCH_PUBSUB(l) ((l)->id)

namespace LM {
	class Publisher;

	union PubSub {
		long id;
		const char* p;
	};

	class Subscriber {
	public:
		virtual ~Subscriber() {}
		virtual void handle_pub(PubSub type, Publisher* pub, void* data) = 0;
	};

	class Requestable {
	public:
		virtual ~Requestable() {}
		virtual void handle_req(PubSub type, void* data) = 0;
	};

	class ReqSub : public Subscriber, public Requestable {
	public:
		struct Reprocessor {
			virtual ~Reprocessor() {}
			virtual void* operator()(const char* signature, void* data) = 0;
			virtual void clean(void* mess) = 0;
		};

	private:
		std::map<long, std::pair<long, Reprocessor*> > m_reproc;

	public:
		virtual ~ReqSub();

		void rebind(PubSub old_type, PubSub new_type, Reprocessor* proc = NULL);
		virtual void handle_pub(PubSub type, Publisher* pub, void* data);
	};

	class Publisher {
	private:
		std::map<long, std::pair<Subscriber*, void*> > m_subs;

	protected:
		void raise(PubSub type);

	public:
		virtual ~Publisher() {}

		void set_subscriber(PubSub type, Subscriber* sub, void* data);
	};
}

#endif
