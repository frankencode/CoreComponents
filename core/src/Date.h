/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUX_DATE_H
#define FLUX_DATE_H

#include <flux/String>

namespace flux {

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

/** \brief Gregorian calendar dates
  */
class Date: public Object
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

    inline Ref<Date> copy() const { return new Date(*this); }

    inline bool isValid() const { return time_ != flux::nan;}

    inline int year() const { return year_; }
    inline int month() const { return month_; }
    inline int day() const { return day_; }
    inline int weekDay() const { return weekDay_; }
    inline int yearDay() const { return yearDay_; }
    inline int hour() const { return hour_; }
    inline int minutes() const { return minutes_; }
    inline int seconds() const { return seconds_; }
    inline int offset() const { return offset_; }

    double time() const;
    String toString() const;

    String monthName() const;
    String dayName() const;

private:
    Date();
    Date(const Date &b);
    Date(double time, int offset);
    Date(int year, int month, int day, int hour, int minutes, int seconds, int offset);

    int year_;
    int month_;
    int day_;
    int weekDay_;
    int yearDay_;
    int hour_;
    int minutes_;
    int seconds_;
    int offset_;

    double time_;
};

} // namespace flux

#endif // FLUX_DATE_H
