/****************************************************************************
**
** This file is part of libPONA - The Portable Network Abstraction Library.
**
** Copyright (C) 2007-2009  Frank Mertens
**
** This file is part of a free software: you can redistribute it and/or
** modify it under the terms of the GNU General Public License as published
** by the Free Software Foundation, either version 3 of the License,
** or (at your option) any later version.
**
** The library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this libary.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

#ifndef PONA_CLOCK_HPP
#define PONA_CLOCK_HPP

#ifdef _WIN32
#include <windows.h>
#include <mmsystem.h>
#else
#include <sys/time.h>
#include <time.h>    // Linux workaround
#endif

#include "TimeStamp.hpp"

namespace pona
{

#ifdef _WIN32

inline TimeStamp getTime()
{
	int ms = timeGetTime();
	return TimeStamp(ms/1000, (ms%1000)*1000000);
}

#else

inline TimeStamp getTime()
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

#endif

class Clock
{
public:
	inline void start() { t0 = getTime(); }
	inline TimeStamp elapsed() const { return getTime() - t0; }
	inline TimeStamp restart()
	{
		TimeStamp t = getTime();
		TimeStamp dt = t - t0;
		t0 = t;
		return dt;
	}
private:
	TimeStamp t0;
};

} // namespace pona

#endif // PONA_CLOCK_HPP
