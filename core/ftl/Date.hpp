 /*
  * Copyright (C) 2007-2013 Frank Mertens.
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version
  * 2 of the License, or (at your option) any later version.
  */
#ifndef FTL_DATE_HPP
#define FTL_DATE_HPP

#include "Time.hpp"
#include "string.hpp"

namespace ftl
{

typedef struct tm StructTm;

/** \brief broken-down time
  * \see Time
  */
class Date: public StructTm, public Instance
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

	inline static hook<Date> create() {
		return new Date;
	}
	inline static hook<Date> create(Time time) {
		return new Date(time);
	}

	// \todo
	// improve logic (add date validation, bool validated_, etc...)
	inline bool valid() const { return tm_off != -2;}

	static hook<Date> localTime();
	static hook<Date> localTime(Time time);

	inline int year() const { return tm_year + 1900; }
	inline int month() const { return tm_mon; }
	inline int day() const { return tm_mday; }
	inline int weekday() const { return tm_wday; }
	inline int yearday() const { return tm_yday; }
	inline int hour() const { return tm_hour; }
	inline int minute() const { return tm_min; }
	inline int second() const { return tm_sec; }
	inline int offset() const { return tm_off; }

	Time time() const;

	// \todo
	string toString() const;
	// static hook<Date> fromString(string s);

	// \todo
	// local time formatting

private:
	Date();
	Date(Time time);

	// \todo
	// Date(string s);
	// Date(int year, int month, int day, int hour = 0, int min = 0, int sec = 0);

	void clear();
	void init(Time time);

	// \todo: replace by a unixDay(;)
	static int julianDay(int year, int month, int day);
	int tm_off;
	// \todo: fully enable ms resolution (ss.fraction in string output)
	// int tm_ms;
};

} // namespace ftl

#endif // FTL_DATE_HPP
