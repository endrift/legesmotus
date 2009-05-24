/*
 * common/timer.cpp
 *
 * Copyright 2009 - Nathan Partlan, Andrew Ayer, Daniel Schneider, and Jeffrey Pfau
 * 
 */

#include "timer.hpp"
#include <stddef.h>

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

uint64_t get_ticks() {
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

uint64_t get_ticks() {
	static const TimeOfDay	start;
	const TimeOfDay		now;

	return (now.tv.tv_sec - start.tv.tv_sec) * 1000ULL + (now.tv.tv_usec - start.tv.tv_usec) / 1000;
}

#endif
