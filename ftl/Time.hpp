/*
 * Time.hpp -- wide-range, high resolution time representation
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../COPYING for the license.
 */
#ifndef FTL_TIME_HPP
#define FTL_TIME_HPP

#include <sys/time.h>
#include <time.h>    // Linux workaround
#include "types.hpp"

namespace ftl
{

/** A 'Time' stores two integers: number of seconds and fractional number of nano-seconds.
  * Objects of type 'Time' are fully compatible to floating point seconds.
  * The time scale has nanosecond resolution and ranges from
  * -68 to +68 years on 32 bit systems and -292e9 to +292e9 years on 64 bit systems.
  */
class Time
{
public:
	Time() {}
	Time(const Time& b) { *this = b; }
	Time(int sec, int nsec = 0): sec_(sec), nsec_(nsec) {}
	Time(float fineSec) { *this = fineSec; }
	Time(double fineSec) { *this = fineSec; }
	
	inline const Time& operator=(const Time& b) {
		sec_ = b.sec_; nsec_ = b.nsec_;
		return *this;
	}

	inline const Time& operator=(int sec) {
		sec_ = sec; nsec_ = 0;
		return *this;
	}
	
	inline const Time& operator=(float fineSec) {
		return *this = double(fineSec);
	}
	
	inline const Time& operator=(double fineSec) {
		sec_ = int(fineSec); nsec_ = int(1000000000 * (fineSec - sec_));
		return *this;
	}
	
	inline operator float() const { return fineSec(); }
	inline operator double() const { return fineSec(); }

	inline int sec() const { return sec_; }
	inline int nsec() const { return nsec_; }
	inline int usec() const { return nsec_ / 1000; }
	
	inline int d() const { return s() / (3600 * 24) + (s() % (3600 * 24) >= (1800 * 24)); }
	inline int min() const { return s() / 60 + (s() % 60 >= 30); }
	inline int s() const { return sec_ + (nsec_ >= 500000000); }
	inline uint64_t ms() const { return uint64_t(sec_) * 1000 + nsec_ / 1000000; }
	inline uint64_t us() const { return uint64_t(sec_) * 1000000 + nsec_ / 1000; }
	inline double fineSec() const { return double(sec_) + double(nsec_)/1000000000; }
	
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
	
	template<class B>
	inline const Time& operator+=(B b) { return *this += Time(b); }
	
private:
	int sec_, nsec_;
};

inline Time operator+(const Time& a, const Time& b) {
	Time c = a;
	return c += b;
}

inline Time operator-(const Time& a, const Time& b) {
	Time c = a;
	return c -= b;
}

inline double operator/(const Time& a, const Time& b) {
	return a.fineSec() / b.fineSec();
}

template<class B>
inline Time operator+(const Time& a, B b) {
	Time c = a;
	return c += b;
}

template<class B>
inline Time operator-(const Time& a, B b) {
	Time c = a;
	return c -= b;
}

inline bool operator==(const Time& a, const Time& b) { return ((a.sec() == b.sec()) && (a.nsec() == b.nsec())); }
inline bool operator!=(const Time& a, const Time& b) { return !(a == b); }
inline bool operator<(const Time& a, const Time& b) { return (a.sec() < b.sec()) || ((a.sec() == b.sec()) && (a.nsec() < b.nsec())); }
inline bool operator>(const Time& a, const Time& b) { return (a.sec() > b.sec()) || ((a.sec() == b.sec()) && (a.nsec() > b.nsec())); }
inline bool operator<=(const Time& a, const Time& b) { return (a < b) || (a == b); }
inline bool operator>=(const Time& a, const Time& b) { return (a > b) || (a == b); }

inline bool operator==(const Time& a, int b) { return a == Time(b); }
inline bool operator!=(const Time& a, int b) { return a != Time(b); }
inline bool operator<(const Time& a, int b) { return a < Time(b); }
inline bool operator>(const Time& a, int b) { return a > Time(b); }
inline bool operator<=(const Time& a, int b) { return a <= Time(b); }
inline bool operator>=(const Time& a, int b) { return a >= Time(b); }

inline bool operator==(const Time& a, float b) { return a == Time(b); }
inline bool operator!=(const Time& a, float b) { return a != Time(b); }
inline bool operator<(const Time& a, float b) { return a < Time(b); }
inline bool operator>(const Time& a, float b) { return a > Time(b); }
inline bool operator<=(const Time& a, float b) { return a <= Time(b); }
inline bool operator>=(const Time& a, float b) { return a >= Time(b); }

inline bool operator==(const Time& a, double b) { return a == Time(b); }
inline bool operator!=(const Time& a, double b) { return a != Time(b); }
inline bool operator<(const Time& a, double b) { return a < Time(b); }
inline bool operator>(const Time& a, double b) { return a > Time(b); }
inline bool operator<=(const Time& a, double b) { return a <= Time(b); }
inline bool operator>=(const Time& a, double b) { return a >= Time(b); }

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

} // namespace ftl

#endif // FTL_TIME_HPP
