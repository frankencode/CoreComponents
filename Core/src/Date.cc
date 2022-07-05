/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Date>
#include <cc/System>
#include <cc/Timezone>
#include <cc/str>
#include <limits>
#include <cmath>
#include <cstdint>

namespace cc {

/* Every 4th year is a leap year excluding every last year in the
 * 100 year cycle (century) but not the last year in a 400 year cycle.
 */
bool leapYear(int y)
{
    return (y % 4 == 0) && ((y % 100 != 0) || (y % 400 == 0));
}

/** Number of leap years between x and y (excluding x, y).
  */
static int leaps(int x, int y)
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
int daysInYear(int y)
{
    return 31 * 7 + 30 * 4 + 28 + leapYear(y);
}

int daysInMonth(int i, int y)
{
    const int days[] = {
        31/*Jan*/, 28/*Feb*/, 31/*Mar*/, 30/*Apr*/, 31/*May*/, 30/*Jun*/,
        31/*Jul*/, 31/*Aug*/, 30/*Sep*/, 31/*Oct*/, 30/*Nov*/, 31/*Dec*/
    };
    return days[i] + (i == 1) * leapYear(y);
}

void Date::State::timeToCalendar()
{
    const int C1   = 365;
    const int C4   =  4 * C1 + 1;
    const int C100 = 25 * C4 - 1;
    const int C400 =  4 * C100 + 1;
    const int N0   = 719528; // linear day number of Jan 1st year 0

    std::int64_t t = std::int64_t(time) + offset + std::int64_t(N0) * 86400;
    int n = t / 86400;
    if (n < 0) n = 0;

    hour = (t / 3600) % 24;
    minutes = (t / 60) % 60;
    seconds = t % 60;
    weekDay = (n + 6) % 7;

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

    year = y;
    yearDay = n;

    month = 0;
    while (true) {
        int h = daysInMonth(month, y);
        if (n < h) break;
        ++month;
        n -= h;
    }
    ++month;
    day = n + 1;
}

void Date::State::calendarToTime()
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

    --day;
    --month;

    int64_t t = 0;
    for (int i = 0; i < month; ++i)
        t += daysInMonth(i, year);
    t += day;
    yearDay = t;
    t += (31 * 7 + 30 * 4 + 28) * (year - 1970);
    if (year >= 1970)
        t += leaps(1970, year);
    else
        t -= leaps(1970, year - 1);
    weekDay = (719528 + t + 6) % 7;
    t *= 86400;
    t += 3600 * hour + 60 * minutes + seconds;
    time = t - offset;

    ++day;
    ++month;
}

Date Date::now()
{
    return Date{System::now()};
}

Date Date::localNow()
{
    return local(System::now());
}

Date Date::local(double time)
{
    return Date{time, Timezone::offset(time)};
}

Date::Date():
    me{std::numeric_limits<double>::quiet_NaN()}
{}

Date::Date(double time, int offset):
    me{time, offset}
{
    me().timeToCalendar();
}

Date::Date(int year, int month, int day, int hour, int minutes, int seconds, int offset):
    me{0., offset, year, month, day, hour, minutes, seconds}
{
    me().calendarToTime();
}

bool Date::isValid() const
{
    return !std::isnan(me().time);
}

double Date::nextTime(int hour, int minutes, int seconds) const
{
    assert(offset() == Timezone::offset(time()));

    Date n {
        year(),
        month(),
        day(),
        hour,
        minutes,
        seconds,
        offset()
    };

    double t = n.time();
    if (n <= *this) t += SecondsPerDay;
    t += n.offset() - Timezone::offset(t);

    return t;
}

String Date::monthName() const
{
    const char *names[] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
    int i = month() - 1;
    if (i < 0) i = 0;
    else if (i > 11) i = 11;
    return names[i];
}

String Date::dayName() const
{
    const char *names[] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
    int i = weekDay();
    if (i < 0) i = 0;
    else if (i > 6) i = 6;
    return names[i];
}

String Date::toString() const
{
    String tz = "Z";
    int offset = me().offset / 60;
    if (offset > 0)
        tz = List<String>{} << "+" << dec(offset / 60, 2) << dec(offset % 60, 2);
    else if (offset < 0)
        tz = List<String>{} << "-" << dec((-offset) / 60, 2) << dec((-offset) % 60, 2);

    return List<String>{}
        << dec(me().year, 4) << "-" << dec(me().month, 2) << "-" << dec(me().day, 2)
        << "T" << dec(me().hour, 2) << dec(me().minutes, 2) << dec(me().seconds, 2) << tz;
}

} // namespace cc
