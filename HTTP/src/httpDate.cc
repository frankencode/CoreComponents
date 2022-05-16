/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
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
        String s = parts.at(1);
        for (int i = 0; i < s.count(); ++i) {
            char ch = s.at(i);
            day *= 10;
            if ('0' <= ch && ch <= '9') day += ch - '0';
            else return Date{0};
        }
        if (day > 31) return Date{};
    }
    int month = 0;
    {
        const char *names[] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
        String s = parts.at(2);
        for (; month < 12; ++month) {
            if (s == names[month]) break;
        }
        if (month > 11) return Date{};
        ++month;
    }
    int year = 0;
    {
        String s = parts.at(3);
        for (int i = 0; i < s.count(); ++i) {
            char ch = s.at(i);
            year *= 10;
            if ('0' <= ch && ch <= '9') year += ch - '0';
            else return Date{};
        }
    }
    int hour = 0, minutes = 0, seconds = 0;
    {
        String s = parts.at(4);
        int *p = &hour;
        for (int i = 0; i < s.count(); ++i) {
            char ch = s.at(i);
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
