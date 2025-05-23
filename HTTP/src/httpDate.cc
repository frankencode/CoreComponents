/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/httpDate>
#include <cc/Format>

namespace cc {

String httpDateToString(const Date &date)
{
    return Format{}
        << date.dayName() << ", "
        << dec(date.day(), 2) << " " << date.monthName() << " " << date.year() << " "
        << dec(date.hour(), 2) << ":" << dec(date.minutes(), 2) << ":" << dec(date.seconds(), 2) << " GMT";
}

Date httpDateFromString(const String &text, Out<bool> ok)
{
    List<String> parts = text.split(' ');
    ok = false;
    if (parts.count() != 6) return Date{};
    int day = 0;
    {
        for (const char &ch: parts.at(1)) {
            day *= 10;
            if ('0' <= ch && ch <= '9') day += ch - '0';
            else return Date{0};
        }
        if (day > 31) return Date{};
    }
    int month = 0;
    {
        const char *names[] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
        const String &s = parts.at(2);
        for (; month < 12; ++month) {
            if (s == names[month]) break;
        }
        if (month > 11) return Date{};
        ++month;
    }
    int year = 0;
    {
        for (const char &ch: parts.at(3)) {
            year *= 10;
            if ('0' <= ch && ch <= '9') year += ch - '0';
            else return Date{};
        }
    }
    int hour = 0, minutes = 0, seconds = 0;
    {
        int *p = &hour;
        for (const char &ch: parts.at(4)) {
            if (ch == ':') {
                if (p == &hour) p = &minutes;
                else if (p == &minutes) p = &seconds;
                else return Date{};
            }
            else {
                *p *= 10;
                if ('0' <= ch && ch <= '9') *p += ch - '0';
                else return Date{};
            }
        }
    }
    ok = true;
    return Date{year, month, day, hour, minutes, seconds};
}

} // namespace cc
