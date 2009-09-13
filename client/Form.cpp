/*
 * client/Form.cpp
 *
 * This file is part of Leges Motus, a networked, 2D shooter set in zero gravity.
 * 
 * Copyright 2009 Andrew Ayer, Nathan Partlan, Jeffrey Pfau
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

#include "Form.hpp"

using namespace LM;
using namespace std;

void Form::reset() {
	for (map<string, FormItem*>::const_iterator iter = m_items.begin(); iter != m_items.end(); ++iter) {
		iter->second->reset();
	}
}

void Form::add_item(const string& name, FormItem* item) {
	m_items[name] = item;
}

void Form::remove_item(const string& name) {
	m_items.erase(name);
}

FormItem* Form::get_item(const string& name) {
	if (m_items.find(name) != m_items.end()) {
		return m_items[name];
	} else {
		return NULL;
	}
}
