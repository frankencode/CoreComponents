/****************************************************************************
**
** This file is part of libPONA - The Portable Network Abstractions Library.
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

#ifndef PONA_TIMESTAMP_HPP
#define PONA_TIMESTAMP_HPP

namespace pona
{

/** \brief Wide-range, high resolution time representation
  *
  * A TimeStamp stores two integers: seconds and fractional number of nano-seconds.
  * Objects of type TimeStamp are fully compatible to a floating point seconds.
  * Thereby the time scale has nano-second resolution and ranges from
  * -68 to +68 years on 32 bit systems and -292e9 to +292e9 years on 64 bit systems.
  */
class TimeStamp
{
public:
	TimeStamp() {}
	TimeStamp(int seconds, int nanoSeconds): sec_(seconds), nsec_(nanoSeconds) {}
	TimeStamp(const TimeStamp& b) { *this = b; }
	TimeStamp(float fineSeconds) { *this = fineSeconds; }
	
	inline const TimeStamp& operator=(const TimeStamp& b)
	{
		sec_ = b.sec_; nsec_ = b.nsec_;
		return *this;
	}

	inline const TimeStamp& operator=(float fineSec)
	{
		sec_ = int(fineSec); nsec_ = int(1000000000 * (fineSec - sec_));
		return *this;
	}
	
	inline operator float() const { return fineSeconds(); }

	inline int secondsPart() const { return sec_; }
	inline int nanoSecondsPart() const { return nsec_; }
	
	inline int microSeconds() const { return sec_ * 1000000 + nsec_ / 1000; }
	inline int miliSeconds() const { return sec_ * 1000 + nsec_ / 1000000; }
	inline float fineSeconds() const { return float(sec_) + float(nsec_)/1000000000; }

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
