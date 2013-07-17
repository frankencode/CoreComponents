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

#include "String.h"

namespace fkit
{

typedef struct tm StructTm;

/** \brief broken-down time
  * \see Time
  */
class Date: public StructTm, public Object
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

	inline static Ref<Date> create() {
		return new Date;
	}
	inline static Ref<Date> create(double time) {
		return new Date(time);
	}

	Ref<Date> copy() const;

	inline bool valid() const { return time_ != fkit::nan;}

	static Ref<Date> localTime();
	static Ref<Date> localTime(double time);

	inline int year() const { return tm_year + 1900; }
	inline int month() const { return tm_mon; }
	inline int day() const { return tm_mday; }
	inline int weekday() const { return tm_wday; }
	inline int yearday() const { return tm_yday; }
	inline int hour() const { return tm_hour; }
	inline int minute() const { return tm_min; }
	inline int second() const { return tm_sec; }
	inline int offset() const { return tm_off; }

	double time() const;

	String toString() const;

private:
	Date();
	Date(double time);

	void clear();
	void init(double time);

	int tm_off;

	double time_;
};

} // namespace fkit

#endif // FKIT_DATE_H
