/*
 * gui/ResourceCache.cpp
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

#include "ResourceCache.hpp"
#include "Image.hpp"
#include "common/Exception.hpp"
#include <iostream>

using namespace LM;
using namespace std;

ResourceCache::ResourceCache(const string& root, DrawContext* ctx) {
	m_ctx = ctx;
	m_root = root;
}

ResourceCache::~ResourceCache() {
	free_all_unused();

	#ifdef LM_DEBUG
	if (!m_instances_image.empty() || !m_instances_font.empty()) {
		for (instance_map<Image>::const_iterator iter = m_instances_image.begin(); iter != m_instances_image.end(); ++iter) {
			cerr << iter->first << " still live" << endl;
		}
		throw new Exception("Resources still in use while freeing cache");
	}
	#endif
}

template<>
ResourceCache::instance_map<Image>& ResourceCache::get_instances<Image>() {
	return m_instances_image;
}

template<>
ResourceCache::instance_map<Font>& ResourceCache::get_instances<Font>() {
	return m_instances_font;
}

void ResourceCache::free_all_unused() {
	free_unused<Image>();
	free_unused<Font>();
}

DrawContext::Image ResourceCache::get_image_handle(const string& name, bool autogen) {
	Image* image = get<Image>(name);
	if (image == NULL) {
		return 0;
	}
	DrawContext::Image handle = image->get_handle();
	if (handle == 0 && autogen) {
		handle = image->gen_handle();
	}
	return handle;
}

void ResourceCache::set_context(DrawContext* ctx) {
	m_ctx = ctx;
}

const string& ResourceCache::get_root() {
	return m_root;
}

DrawContext* ResourceCache::get_context() {
	return m_ctx;
}
