/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include <unistd.h> // sysconf(3), alarm(2)
#ifndef __linux
#include <sys/types.h>
#ifdef __OpenBSD__
#include <sys/param.h>
#endif
#include <sys/sysctl.h>
#endif
#include <sys/time.h>
#include <time.h>
#include "System.h"

namespace fkit
{

int System::concurrency()
{
	#ifdef _SC_NPROCESSORS_ONLN
		return sysconf(_SC_NPROCESSORS_ONLN);
	#endif

	#ifdef _SC_NPROC_ONLN
		return sysconf(_SC_NPROC_ONLN);
	#endif

	#ifndef __linux
	#ifdef HW_NCPU
	int n = 1;
	size_t sz = sizeof(n);
	int mib[4];
	mib[0] = CTL_HW;
	mib[1] = HW_NCPU;
	sysctl(mib, 2, &n, &sz, NULL, 0);
	if(n > 1) return n;
	#endif
	#endif

	return 1;
}

int System::pageSize()
{
	return sysconf(_SC_PAGE_SIZE);
}

double System::now()
{
#if _POSIX_TIMERS > 0
	struct timespec ts;
	::clock_gettime(CLOCK_REALTIME, &ts);
	return double(ts.tv_sec) + double(ts.tv_nsec) / 1e9;
#else
	struct timeval tv;
	::gettimeofday(&tv, 0);
	return double(tv.tv_sec) + double(tv.tv_usec) / 1e6);
#endif
}

} // namespace fkit
