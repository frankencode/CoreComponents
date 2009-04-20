/*
 * Time.hpp -- wide-range, high resolution time representation
 *
 * Copyright (c) 2007-2009, Frank Mertens
 *
 * See ../LICENSE for the license.
 */
#ifndef PONA_TIME_HPP
#define PONA_TIME_HPP

#include <sys/time.h>
#include <time.h>    // Linux workaround
#include "types.hpp"

namespace pona
{

/** A 'Time' stores two integers: number of seconds and fractional number of nano-seconds.
  * Objects of type 'Time' are fully compatible to floating point seconds.
  * The time scale has nano-second resolution and ranges from
  * -68 to +68 years on 32 bit systems and -292e9 to +292e9 years on 64 bit systems.
  */
class Time
{
public:
	Time() {}
	Time(int seconds, int nanoSeconds): sec_(seconds), nsec_(nanoSeconds) {}
	Time(double fineSeconds) { *this = fineSeconds; }
	Time(const Time& b) { *this = b; }
	
	inline const Time& operator=(const Time& b)
	{
		sec_ = b.sec_; nsec_ = b.nsec_;
		return *this;
	}

	inline const Time& operator=(double fineSec)
	{
		sec_ = int(fineSec); nsec_ = int(1000000000 * (fineSec - sec_));
		return *this;
	}
	
	inline operator double() const { return fineSeconds(); }

	inline int secondsPart() const { return sec_; }
	inline int nanoSecondsPart() const { return nsec_; }
	
	inline int seconds() const { return sec_ + (nsec_ >= 500000000); }
	inline uint64_t miliSeconds() const { return uint64_t(sec_) * 1000 + nsec_ / 1000000; }
	inline uint64_t microSeconds() const { return uint64_t(sec_) * 1000000 + nsec_ / 1000; }
	inline double fineSeconds() const { return double(sec_) + double(nsec_)/1000000000; }
	
	inline int s() const { return seconds(); }
	inline uint64_t ms() const { return miliSeconds(); }
	inline uint64_t us() const { return microSeconds(); }
	
	inline bool operator==(int sec) { return (sec_ == sec) && (nsec_ == 0); }
	inline bool operator!=(int sec) { return !(*this == sec); }
	
	inline const Time& operator+=(const Time& b)
	{
		sec_ += b.sec_;
		nsec_ += b.nsec_;
		if (nsec_ >= 1000000000)
		{
			sec_ += nsec_/1000000000;
			nsec_ %= 1000000000;
		}
		return *this;
	}

	inline const Time& operator-=(const Time& b)
	{
		sec_ -= b.sec_;
		nsec_ -= b.nsec_;
		if (nsec_ <= -1000000000)
		{
			sec_ += nsec_/1000000000;
			nsec_ %= -1000000000;
		}
		return *this;
	}

private:
	int sec_, nsec_;
};

inline Time operator+(const Time& a, const Time& b)
{
	Time c = a;
	return c += b;
}

inline Time operator-(const Time& a, const Time& b)
{
	Time c = a;
	return c -= b;
}

inline bool operator==(const Time& a, const Time& b) { return ((a.secondsPart() == b.secondsPart()) && (a.nanoSecondsPart() == b.nanoSecondsPart())); }
inline bool operator!=(const Time& a, const Time& b) { return !(a == b); }
inline bool operator<(const Time& a, const Time& b) { return (a.secondsPart() < b.secondsPart()) || ((a.secondsPart() == b.secondsPart()) && (a.nanoSecondsPart() < b.nanoSecondsPart())); }
inline bool operator>(const Time& a, const Time& b) { return (a.secondsPart() > b.secondsPart()) || ((a.secondsPart() == b.secondsPart()) && (a.nanoSecondsPart() > b.nanoSecondsPart())); }
inline bool operator<=(const Time& a, const Time& b) { return (a < b) || (a == b); }
inline bool operator>=(const Time& a, const Time& b) { return (a > b) || (a == b); }

inline Time microSeconds(int usec) { return Time(usec/1000000, (usec%1000000)*1000); }
inline Time miliSeconds(int msec) { return Time(msec/1000, (msec%1000)*1000000); }
inline Time seconds(int sec) { return Time(sec, 0); }

inline Time now()
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

} // namespace pona

#endif // PONA_TIME_HPP
