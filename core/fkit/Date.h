/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FKIT_DATE_H
#define FKIT_DATE_H

#ifndef _BSD_SOURCE
#define _BSD_SOURCE
#endif
#include <time.h>
#include "String.h"

namespace fkit
{

enum Weekday {
	Sun = 0,
	Mon = 1,
	Tue = 2,
	Wed = 3,
	Thu = 4,
	Fri = 5,
	Sat = 6
};

enum {
	SecondsPerHour = 3600,
	SecondsPerDay  = 24 * SecondsPerHour
};

inline double hours(double n) { return n * SecondsPerHour; }
inline double days(double n) { return n * SecondsPerDay; }

typedef struct tm StructTm;

/** \brief broken-down time
  * \see Time
  */
class Date: public StructTm, public Object
{
public:
	inline static Ref<Date> create() {
		return new Date;
	}
	inline static Ref<Date> create(double time, int offset = 0) {
		return new Date(time, offset);
	}
	inline static Ref<Date> create(int year, int month, int day, int hour = 0, int minutes = 0, int seconds = 0, int offset = 0) {
		return new Date(year, month, day, hour, minutes, seconds, offset);
	}

	static Ref<Date> now();
	static Ref<Date> localTime();
	static Ref<Date> localTime(double time);

	Ref<Date> copy() const;

	inline bool valid() const { return time_ != fkit::nan;}

	inline int year() const { return tm_year + 1900; }
	inline int month() const { return tm_mon + 1; }
	inline int day() const { return tm_mday; }
	inline int weekday() const { return tm_wday; }
	inline int yearday() const { return tm_yday; }
	inline int hour() const { return tm_hour; }
	inline int minutes() const { return tm_min; }
	inline int seconds() const { return tm_sec; }
	inline int offset() const { return tm_gmtoff; }

	double time() const;
	String toString() const;

	String monthName() const;
	String dayName() const;

private:
	Date();
	Date(double time, int offset);
	Date(int year, int month, int day, int hour, int minutes, int seconds, int offset);

	double time_;
};

} // namespace fkit

#endif // FKIT_DATE_H
