/*
 * Time.cpp -- wide-range, high resolution time representation
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
 */

#include <sys/time.h>
#ifdef __linux
#include <time.h>
#endif
#include "Date.hpp"
#include "Time.hpp"

namespace ftl
{

Time Time::now()
{
#if _POSIX_TIMERS > 0
	struct timespec ts;
	::clock_gettime(CLOCK_REALTIME, &ts);
	return Time(ts.tv_sec, ts.tv_nsec);
#else
	struct timeval tv;
	::gettimeofday(&tv, 0);
	return Time(tv.tv_sec, tv.tv_usec * 1000);
#endif
}

String Time::toString() const {
	return Date::newInstance(*this)->toString();
}

} // namespace ftl
