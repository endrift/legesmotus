/*
 * gui/GraphicContainer.hpp
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

#ifndef LM_GUI_GRAPHICCONTAINER_HPP
#define LM_GUI_GRAPHICCONTAINER_HPP

#include "Widget.hpp"
#include "Graphic.hpp"

#include <map>

namespace LM {
	class GraphicContainer : public Widget {
	private:
		/*
		 * Important! The priority multimap holds the actual elements
		 * The name map essentially hold pointers to within the multimap
		 * This only works because of the fact that map iterators are rarely invalidated
		 * Priority map objects can only removed through the name map, or else
		 * bad things would happen
		 */
		std::multimap<int, Graphic*> m_priority_map;
		std::map<std::string, std::multimap<int, Graphic*>::iterator> m_name_map;

		bool m_autodelete;

	public:
		GraphicContainer(bool self_contained = false, Widget* parent = NULL);
		virtual ~GraphicContainer();

		void add_graphic(Graphic* graphic, int priority = 0);
		void add_graphic(const std::string& name, Graphic* graphic, int priority = 0);

		Graphic* get_graphic(const std::string& name);

		void change_priority(const std::string& name, int new_priority);

		void remove_graphic(const std::string& name);

		virtual void draw(DrawContext* ctx) const;
	};
}

#endif
