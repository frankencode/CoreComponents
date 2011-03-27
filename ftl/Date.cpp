/*
 * Date.cpp -- broken down calendar times
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../COPYING for the license.
 */

#include "streams" // DEBUG
#include "Format.hpp"
#include "Date.hpp"

namespace ftl
{

/* Every 4th year is a leap year excluding every last year in the
 * 100 year cycle (century) but not the last year in a 400 year cycle.
 */
inline bool leapYear(int y)
{
	return (y % 4 == 0) && ((y % 100 != 0) || (y % 400 == 0));
}

/** Minor improvement of the leapYear definition proposed by Sir John Herschel.
  */
inline bool herschelLeapYear(int y)
{
	return leapYear(y) && (y % 4000 != 0);
}

/** Days in a year are arbitrary distributed but weighted to make the
  * average length of a year match the length of the solar year.
  * Interestingly the length of the year is measured in days, which is a duration
  * depending on Earth's angular velocity.
  */
inline int daysInYear(int y)
{
	return 31 * 7 + 30 * 4 + 28 + leapYear(y);
}

inline int daysInMonth(int i, int y)
{
	const int days[] = {
		31/*Jan*/, 28/*Feb*/, 31/*Mar*/, 30/*Apr*/, 31/*May*/, 30/*Jun*/,
		31/*Jul*/, 31/*Aug*/, 30/*Sep*/, 31/*Oct*/, 30/*Nov*/, 31/*Dec*/
	};
	return days[i] + (i == 1) * leapYear(y);
}

Date::Date(): tm_off(-2) { clear(); }
Date::Date(Time time): tm_off(0) { clear(); init(time); }

void Date::clear()
{
	struct tm* tm = this;
	mem::clr(tm, sizeof(struct tm)); // for paranoid reason
}

void Date::init(Time time)
{
	const int t0 = 719162;                    // linear day number (starting from 0) of 1st Jan 1970
	int tx = t0 + time.sec() / SecondsPerDay; // linear day number of this date
	int d400 = 146097;                        // length of the 400 year cycle
	int d100 = 36524;                         // length of the 100 year cycle
	int d4 = 1461;                            // length of the 4 year cycle
	
	int tx400 = tx    % d400; // day within the 400 year cycle (starting from 0)
	int tx100 = tx400 % d100; // day within the 100 year cycle (starting from 0)
	int tx4   = tx100 % d4;   // day within the   4 year cycle (starting from 0)
	
	int c400 = tx    / d400; // 400 year cycle number within continuum (starting from 0)
	int c100 = tx400 / d100; // 100 year cycle number within 400 year cycle (starting from 0)
	int c4   = tx100 / d4;   //   4 year cycle number within 100 year cycle (starting from 0)
	
	int century = (c400 * 400 + c100 * 100) / 100;
	int year = (century * 100) + c4 * 4 + 1;
	int ty0 = 0; // first day in year within the 4 year cycle (starting from 0)
	{
		while (true) {
			int ty1 = ty0 + daysInYear(year); // first day of next year (starting from 0)
			if (tx4 < ty1) break;
			ty0 = ty1;
			++year;
		}
	}
	int tyx = tx4 - ty0; // day witin this year (starting from 0)
	tm_mon = 0;
	int tm0 = 0; // first day of month (within this year)
	{
		for (int i = 0; i < 12; ++i) {
			int tm1 = tm0 + daysInMonth(i, year); // first day of next month
			if (tyx < tm1) break;
			++tm_mon;
			tm0 = tm1;
		}
	}
	tm_mday = tyx - tm0 + 1/*adjust to start with day 1*/;
	tm_wday = (tx + 1/*adjust to start with Sunday*/) % 7;
	tm_yday = tyx;
	
	tm_hour = (time.sec() / 3600) % 24;
	tm_min = (time.sec() / 60) % 60;
	tm_sec = time.sec() % 60;
	
	tm_year = year - 1900;
}

Ref<Date, Owner> Date::localTime()
{
	return localTime(Time::now());
}

Ref<Date, Owner> Date::localTime(Time time)
{
	Date utc(time);
	Ref<Date, Owner> local = new Date;
	time_t seconds = time.sec();
	localtime_r(&seconds, local);
	Time offset = local->time() - utc.time();
	local->tm_off = offset.sec() / 60;
	return local;
}

Time Date::time() const
{
	int64_t t = 0;
	int year = tm_year + 1900;
	for (int y = 1970; year < y; --y)
		t -= daysInYear(y - 1) * SecondsPerDay;
	for (int y = 1970; y < year; ++y)
		t += daysInYear(y) * SecondsPerDay;
	for (int i = 0; i < tm_mon; ++i)
		t += daysInMonth(i, year) * SecondsPerDay;
	t += (tm_mday - 1) * SecondsPerDay;
	t += tm_hour * SecondsPerHour;
	t += tm_min * SecondsPerMinute;
	t += tm_sec;
	return Time(t, 0);
}

String Date::iso8601() const
{
	String tz = "Z";
	if (tm_off > 0)
		tz = Format("+%2.:'0'%%2.:'0'%") << (tm_off / 60) << (tm_off % 60);
	else if (tm_off < 0)
		tz = Format("-%2.:'0'%%2.:'0'%") << ((-tm_off) / 60) << ((-tm_off) % 60);
	
	return Format(
		"%4.:'0'%-%2.:'0'%-%2.:'0'%T"
		"%2.:'0'%%2.:'0'%%2.:'0'%%%"
	) << (tm_year + 1900) << (tm_mon + 1) << tm_mday
	  << tm_hour << tm_min << tm_sec << tz;
}

} // namespace ftl
