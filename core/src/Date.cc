/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <stdint.h>
#include <cc/Format>
#include <cc/Date>

namespace cc {

/* Every 4th year is a leap year excluding every last year in the
 * 100 year cycle (century) but not the last year in a 400 year cycle.
 */
inline bool leapYear(int y)
{
    return (y % 4 == 0) && ((y % 100 != 0) || (y % 400 == 0));
}

/** Number of leap years between x and y (excluding x, y).
  */
int leaps(int x, int y)
{
    if (x > y) {
        int h = x;
        x = y;
        y = h;
    }
    --y; ++x; // exclude year x, y
    if (x > y) return 0;
    return y / 4 - y / 100 + y / 400 - (x - 1) / 4 + (x - 1) / 100 - (x - 1) / 400;
}

/** Days in a year are arbitrarily distributed but weighted to make the
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

Date::Date():
    offset_(0),
    time_(cc::nan)
{}

Date::Date(const Date &b):
    year_(b.year_),
    month_(b.month_),
    day_(b.day_),
    weekDay_(b.weekDay_),
    yearDay_(b.yearDay_),
    hour_(b.hour_),
    minutes_(b.minutes_),
    seconds_(b.seconds_),
    offset_(b.offset_),
    time_(b.time_)
{}

Date::Date(double time, int offset):
    offset_(offset),
    time_(time)
{
    const int C1   = 365;
    const int C4   =  4 * C1 + 1;
    const int C100 = 25 * C4 - 1;
    const int C400 =  4 * C100 + 1;
    const int N0   = 719528; // linear day number of Jan 1st year 0

    int64_t t = int64_t(time) + int64_t(N0) * 86400;
    int n = t / 86400;
    if (n < 0) n = 0;

    hour_ = (t / 3600) % 24;
    minutes_ = (t / 60) % 60;
    seconds_ = t % 60;
    weekDay_ = (n + 6) % 7;

    int y = 400 * (n / C400);
    n = n % C400;

    while (true) {
        int h = C100 + leapYear(y);
        if (n < h) break;
        n -= h;
        y += 100;
    }

    while (true) {
        int h = C4 - !leapYear(y);
        if (n < h) break;
        n -= h;
        y += 4;
    }

    while (true) {
        int h = C1 + leapYear(y);
        if (n < h) break;
        n -= h;
        ++y;
    }

    year_ = y;
    yearDay_ = n;

    month_ = 0;
    while (true) {
        int h = daysInMonth(month_, y);
        if (n < h) break;
        ++month_;
        n -= h;
    }
    ++month_;
    day_ = n + 1;
}

Date::Date(int year, int month, int day, int hour, int minutes, int seconds, int offset):
    year_(year),
    month_(month),
    day_(day),
    hour_(hour),
    minutes_(minutes),
    seconds_(seconds),
    offset_(offset)
{
    if (year < 1) year = 1;
    if (month > 12) month = 12;
    else if (month < 1) month = 1;
    if (day > 31) day = 31;
    else if (day < 1) day = 1;
    if (hour < 0) hour = 0;
    else if (hour > 23) hour = 23;
    if (minutes > 59) minutes = 59;
    else if (minutes < 0) minutes = 0;
    if (seconds > 59) seconds = 59;
    else if (seconds < 0) seconds = 0;
    if (offset > 600) offset = 600;
    else if (offset < -600) offset = -600;

    --day;
    --month;

    int64_t t = 0;
    for (int i = 0; i < month; ++i)
        t += daysInMonth(i, year);
    t += day;
    yearDay_ = t;
    t += (31 * 7 + 30 * 4 + 28) * (year - 1970);
    if (year >= 1970)
        t += leaps(1970, year);
    else
        t -= leaps(1970, year - 1);
    weekDay_ = (719528 + t + 6) % 7;
    t *= 86400;
    t += 3600 * hour + 60 * minutes + seconds;
    time_ = t;
}

double Date::time() const
{
    return time_;
}

String Date::toString() const
{
    //! \todo fully enable ms resolution (ss.fraction in String output)
    //! \todo local time Formatting

    String tz = "Z";
    int offset = offset_ / 60;
    if (offset > 0)
        tz = Format() << "+" << dec(offset / 60, 2) << dec(offset % 60, 2);
    else if (offset < 0)
        tz = Format() << "-" << dec((-offset) / 60, 2) << dec((-offset) % 60, 2);

    return Format()
        << dec(year_, 4) << "-" << dec(month_, 2) << "-" << dec(day_, 2)
        << "T" << dec(hour_, 2) << dec(minutes_, 2) << dec(seconds_, 2) << tz;
}

String Date::monthName() const
{
    const char *names[] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
    int i = month_ - 1;
    if (i < 0) i = 0;
    else if (i > 11) i = 11;
    return names[i];
}

String Date::dayName() const
{
    const char *names[] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
    int i = weekDay_;
    if (i < 0) i = 0;
    else if (i > 6) i = 6;
    return names[i];
}

} // namespace cc
