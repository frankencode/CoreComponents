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

enum {
	SecondsPerMinute = 60,
	SecondsPerHour = 60 *  SecondsPerMinute,
	SecondsPerDay = 24 *  SecondsPerHour
};

/** \brief Wide-range, high resolution time representation
  *
  * Time objects represent nano-second precise time values ranging from -292e9 to +292e9 years.
  * Time::now() delivers the number of nano-seconds since the begin of Epoch (00:00:00 January 1, 1970, UTC).
  * Initializing a time object without argument will create an invalid time object.
  * The internal representation can be accessed using the sec() and nsec() methods.
  */
class Time
{
public:
	Time(): sec_(-1), nsec_(+1) {}
	Time(const Time& b) { *this = b; }
	Time(int64_t sec, int32_t nsec): sec_(sec), nsec_(nsec) {}
	
	Time(int seconds): sec_(seconds), nsec_(0) {}
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
	
	inline bool valid() const {
		return (sec_ == 0) || ((sec_ > 0) && (nsec_ >= 0)) || ((sec_ < 0) && (nsec_ <= 0));
	}
	
	inline int64_t sec() const { return sec_; }
	inline int32_t nsec() const { return nsec_; }
	inline int32_t usec() const { return nsec_ / 1000; }
	
	inline operator bool() const { return valid(); }
	inline operator int() const { return s(); }
	inline operator float() const { return fineSec(); }
	inline operator double() const { return fineSec(); }
	
	inline int d() const { return s() / SecondsPerDay + (s() % SecondsPerDay >= SecondsPerDay / 2); }
	inline int h() const { return s() / SecondsPerHour + (s() % SecondsPerHour >= SecondsPerHour / 2); }
	inline int min() const { return s() / SecondsPerMinute + (s() % SecondsPerMinute >= SecondsPerMinute / 2); }
	inline int s() const { return sec_ + (nsec_ >= 500000000); }
	inline int64_t ms() const { return sec_ * 1000 + nsec_ / 1000000; }
	inline int64_t us() const { return sec_ * 1000000 + nsec_ / 1000; }
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
	
	inline static Time microSeconds(int usec) { return Time(usec/1000000, (usec%1000000)*1000); }
	inline static Time miliSeconds(int msec) { return Time(msec/1000, (msec%1000)*1000000); }
	inline static Time seconds(int sec) { return Time(sec, 0); }
	
	inline static Time now()
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
	
private:
	int64_t sec_;
	int32_t nsec_;
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

} // namespace ftl

#endif // FTL_TIME_HPP
