/*
 * time.hpp -- reading the system time
 *
 * Copyright (c) 2007-2009, Frank Mertens
 *
 * See ../LICENSE for the license.
 */
#ifndef PONA_TIME_HPP
#define PONA_TIME_HPP

#include <sys/time.h>
#include <time.h>    // Linux workaround
#include "TimeStamp.hpp"

namespace pona
{

inline TimeStamp now()
{
#if _POSIX_TIMERS > 0
	struct timespec ts;
	::clock_gettime(CLOCK_REALTIME, &ts);
	return TimeStamp(ts.tv_sec, ts.tv_nsec);
#else
	struct timeval tv;
	::gettimeofday(&tv, 0);
	return TimeStamp(tv.tv_sec, tv.tv_usec * 1000);
#endif
}

} // namespace pona

#endif // PONA_TIME_HPP
