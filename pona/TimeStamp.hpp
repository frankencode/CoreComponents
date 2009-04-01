/*
 * TimeStamp.hpp -- wide-range, high resolution time representation
 *
 * Copyright (c) 2007-2009, Frank Mertens
 *
 * See ../LICENSE for the license.
 */
#ifndef PONA_TIMESTAMP_HPP
#define PONA_TIMESTAMP_HPP

namespace pona
{

/** A TimeStamp stores two integers: number of seconds and fractional number of nano-seconds.
  * Objects of type TimeStamp are fully compatible to a floating point seconds.
  * Tthe time scale has nano-second resolution and ranges from
  * -68 to +68 years on 32 bit systems and -292e9 to +292e9 years on 64 bit systems.
  */
class TimeStamp
{
public:
	TimeStamp() {}
	TimeStamp(int seconds, int nanoSeconds): sec_(seconds), nsec_(nanoSeconds) {}
	TimeStamp(double fineSeconds) { *this = fineSeconds; }
	TimeStamp(const TimeStamp& b) { *this = b; }
	
	inline const TimeStamp& operator=(const TimeStamp& b)
	{
		sec_ = b.sec_; nsec_ = b.nsec_;
		return *this;
	}

	inline const TimeStamp& operator=(double fineSec)
	{
		sec_ = int(fineSec); nsec_ = int(1000000000 * (fineSec - sec_));
		return *this;
	}
	
	inline operator double() const { return fineSeconds(); }

	inline int secondsPart() const { return sec_; }
	inline int nanoSecondsPart() const { return nsec_; }
	
	inline int microSeconds() const { return sec_ * 1000000 + nsec_ / 1000; }
	inline int miliSeconds() const { return sec_ * 1000 + nsec_ / 1000000; }
	inline double fineSeconds() const { return double(sec_) + double(nsec_)/1000000000; }

	inline bool operator==(int sec)
	{
		return (sec_ == sec) && (nsec_ == 0);
	}
	
	inline const TimeStamp& operator+=(const TimeStamp& b)
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

	inline const TimeStamp& operator-=(const TimeStamp& b)
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

inline TimeStamp operator+(const TimeStamp& a, const TimeStamp& b)
{
	TimeStamp c = a;
	return c += b;
}

inline TimeStamp operator-(const TimeStamp& a, const TimeStamp& b)
{
	TimeStamp c = a;
	return c -= b;
}

inline bool operator==(const TimeStamp& a, const TimeStamp& b) { return ((a.secondsPart() == b.secondsPart()) && (a.nanoSecondsPart() == b.nanoSecondsPart())); }
inline bool operator!=(const TimeStamp& a, const TimeStamp& b) { return !(a == b); }
inline bool operator<(const TimeStamp& a, const TimeStamp& b) { return (a.secondsPart() < b.secondsPart()) || ((a.secondsPart() == b.secondsPart()) && (a.nanoSecondsPart() < b.nanoSecondsPart())); }
inline bool operator>(const TimeStamp& a, const TimeStamp& b) { return (a.secondsPart() > b.secondsPart()) || ((a.secondsPart() == b.secondsPart()) && (a.nanoSecondsPart() > b.nanoSecondsPart())); }
inline bool operator<=(const TimeStamp& a, const TimeStamp& b) { return (a < b) || (a == b); }
inline bool operator>=(const TimeStamp& a, const TimeStamp& b) { return (a > b) || (a == b); }

inline TimeStamp microSeconds(int usec) { return TimeStamp(usec/1000000, (usec%1000000)*1000); }
inline TimeStamp miliSeconds(int msec) { return TimeStamp(msec/1000, (msec%1000)*1000000); }
inline TimeStamp seconds(int sec) { return TimeStamp(sec, 0); }

} // namespace pona

#endif // PONA_TIMESTAMP_HPP
