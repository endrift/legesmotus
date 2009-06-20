/*
 * common/timer.cpp
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

#include "timer.hpp"
#include <stddef.h>

using namespace LM;
using namespace std;

#ifdef __WIN32

#include <Windows.h>

namespace {
	uint64_t get_performance_frequency() {
		LARGE_INTEGER li;
		QueryPerformanceFrequency(&li);
		return li.QuadPart / 1000ULL;
	}
	uint64_t get_performance_counter() {
		LARGE_INTEGER li;
		QueryPerformanceCounter(&li);
		return li.QuadPart;
	}
}

uint64_t LM::get_ticks() {
	static const uint64_t	frequency(get_performance_frequency());
	static const uint64_t	start(get_performance_counter());
	const uint64_t		now(get_performance_counter());

	return (now - start) / frequency;
}

#else

#include <sys/time.h>

namespace {
	struct TimeOfDay {
		struct timeval tv;
		TimeOfDay() {
			gettimeofday(&tv, NULL);
		}
	};
}

uint64_t LM::get_ticks() {
	static const TimeOfDay	start;
	const TimeOfDay		now;

	return (now.tv.tv_sec - start.tv.tv_sec) * 1000ULL + (now.tv.tv_usec - start.tv.tv_usec) / 1000;
}

#endif
