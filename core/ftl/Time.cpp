 /*
  * Copyright (C) 2007-2013 Frank Mertens.
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version
  * 2 of the License, or (at your option) any later version.
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
	return Date::create(*this)->toString();
}

} // namespace ftl
