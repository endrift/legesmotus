/*
 * gui/ResourceCache.hpp
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

#ifndef LM_GUI_RESOURCECACHE_HPP
#define LM_GUI_RESOURCECACHE_HPP

#include "DrawContext.hpp"
#include "Font.hpp"
#include "common/Exception.hpp"

#include <string>
#include <map>

namespace LM {
	class ResourceCache;
	class Image;
	// XXX remove Font include when refactor is done
	//class Font;

	class ResourceCache {
	private:
		// For convenience
		template <typename T> class instance_map : public std::map<const std::string, std::pair<T*, int> > {};

		DrawContext*	m_ctx;

		std::string m_root;

		instance_map<Image> m_instances_image;
		instance_map<Font> m_instances_font;

		template<typename T> instance_map<T>& get_instances();

	public:
		ResourceCache(const std::string& root, DrawContext* ctx = NULL);
		~ResourceCache();

		void free_all_unused();

		template<typename T> void add(const std::string& name, const T& res);
		template<typename T> T* get(const std::string& name);
		template<typename T> int increment(const std::string& name);
		template<typename T> int decrement(const std::string& name);
		template<typename T> bool try_free(const std::string& name);
		template<typename T> void free_unused();

		DrawContext::Image get_image_handle(const std::string& name, bool autogen = true);

		void set_context(DrawContext* ctx);

		const std::string& get_root();
		DrawContext* get_context();
	};

	template<> ResourceCache::instance_map<Image>& ResourceCache::get_instances<Image>();

	template<> ResourceCache::instance_map<Font>& ResourceCache::get_instances<Font>();

	template<typename T>
	void ResourceCache::add(const std::string& name, const T& res) {
		instance_map<T>& instances = get_instances<T>();
		if (name.empty()) {
			throw new Exception("Can't add a nameless resource!");
		}

		if (instances.find(name) == instances.end()) {
			instances[name] = make_pair(new T(res), 1);
		}
	}
	
	template<typename T>
	T* ResourceCache::get(const std::string& name) {
		instance_map<T>& instances = get_instances<T>();
		if (instances.find(name) == instances.end()) {
			return NULL;
		}
		return instances[name].first;
	}
	
	template<typename T>
	int ResourceCache::increment(const std::string& name) {
		instance_map<T>& instances = get_instances<T>();
		if (instances.find(name) == instances.end()) {
			return 0;
		}
		return ++instances[name].second;
	}
	
	template<typename T>
	int ResourceCache::decrement(const std::string& name) {
		instance_map<T>& instances = get_instances<T>();
		if (instances.find(name) == instances.end()) {
			return 0;
		}
		return --instances[name].second;
	}
	
	template<typename T>
	bool ResourceCache::try_free(const std::string& name) {
		instance_map<T>& instances = get_instances<T>();
		typename instance_map<T>::iterator iter = instances.find(name);
		if (iter == instances.end()) {
			return true;
		}
		if (iter->second.second <= 0) {
			delete iter->second.first;
			instances.erase(iter);
			return true;
		}
		return false;
	}
	
	template<typename T>
	void ResourceCache::free_unused() {
		instance_map<T>& instances = get_instances<T>();
		for (typename instance_map<T>::iterator iter = instances.begin(); iter != instances.end(); ++iter) {
			if (iter->second.second <= 0) {
				delete iter->second.first;
				instances.erase(iter);
			}
		}
	}
}

#endif
