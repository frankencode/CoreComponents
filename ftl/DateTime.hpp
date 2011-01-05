/*
 * DateTime.hpp -- broken down calendar times
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../COPYING for the license.
 */
#ifndef FTL_DATETIME_HPP
#define FTL_DATETIME_HPP

#include "atoms"
#include "Time.hpp"
#include "String.hpp"

namespace ftl
{

class DateTime: public Instance
{
public:
	enum Weekday {
		Sun = 0,
		Mon = 1,
		Tue = 2,
		Wed = 3,
		Thu = 4,
		Fri = 5,
		Sat = 6
	};
	
	DateTime(Time time);
	
	inline int year() const { return year_; }
	inline int month() const { return month_; }
	inline int day() const { return day_; }
	inline int weekday() const { return weekday_; }
	
	inline int hour() const { return hour_; }
	inline int min() const { return min_; }
	inline int s() const { return s_; }
	inline int ms() const { return ms_; }
	
	String iso8601() const;
	
private:
	Time time_;
	int year_;
	int month_;
	int day_;
	int weekday_;
	int hour_;
	int min_;
	int s_;
	int ms_;
};

} // namespace ftl

#endif // FTL_DATETIME_HPP
