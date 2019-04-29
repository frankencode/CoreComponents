/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Date>
#include <cc/Format>
#include <cc/http/utils>

namespace cc {
namespace http {

const char *reasonPhraseByStatusCode(int statusCode)
{
    const char *phrase = "";
    switch (statusCode) {
    case 100: phrase = "Continue"; break;
    case 101: phrase = "Switching Protocols"; break;
    case 200: phrase = "OK"; break;
    case 201: phrase = "Created"; break;
    case 202: phrase = "Accepted"; break;
    case 203: phrase = "Non-Authoritative Information"; break;
    case 204: phrase = "No Content"; break;
    case 205: phrase = "Reset Content"; break;
    case 206: phrase = "Partial Content"; break;
    case 300: phrase = "Multiple Choices"; break;
    case 301: phrase = "Moved Permanently"; break;
    case 302: phrase = "Moved Temporarily"; break;
    case 303: phrase = "See Other"; break;
    case 304: phrase = "Not Modified"; break;
    case 305: phrase = "Use Proxy"; break;
    case 400: phrase = "Bad Request"; break;
    case 401: phrase = "Unauthorized"; break;
    case 402: phrase = "Payment Required"; break;
    case 403: phrase = "Forbidden"; break;
    case 404: phrase = "Not Found"; break;
    case 405: phrase = "Method Not Allowed"; break;
    case 406: phrase = "Not Acceptable"; break;
    case 407: phrase = "Proxy Authentication Required"; break;
    case 408: phrase = "Request Timeout"; break;
    case 409: phrase = "Conflict"; break;
    case 410: phrase = "Gone"; break;
    case 411: phrase = "Length Required"; break;
    case 412: phrase = "Precondition Failed"; break;
    case 413: phrase = "Request Entity Too Large"; break;
    case 414: phrase = "Request-URI Too Long"; break;
    case 415: phrase = "Unsupported Media Type"; break;
    case 500: phrase = "Internal Server Error"; break;
    case 501: phrase = "Not Implemented"; break;
    case 502: phrase = "Bad Gateway"; break;
    case 503: phrase = "Service Unavailable"; break;
    case 504: phrase = "Gateway Timeout"; break;
    case 505: phrase = "HTTP Version Not Supported"; break;
    }
    return phrase;
}

String formatDate(Date *date)
{
    return Format()
        << date->dayName() << ", "
        << dec(date->day(), 2) << " " << date->monthName() << " " << date->year() << " "
        << dec(date->hour(), 2) << ":" << dec(date->minutes(), 2) << ":" << dec(date->seconds(), 2) << " GMT";
}

Ref<Date> scanDate(const String &text, bool *ok)
{
    // e.g.: Tue, 10 Sep 2013 11:01:10 GMT
    Ref<StringList> parts = text->split(' ');
    if (ok) *ok = false;
    if (parts->count() != 6) return 0;
    int day = 0;
    {
        String s = parts->at(1);
        for (int i = 0; i < s->count(); ++i) {
            char ch = s->at(i);
            day *= 10;
            if ('0' <= ch && ch <= '9') day += ch - '0';
            else return 0;
        }
        if (day > 31) return 0;
    }
    int month = 0;
    {
        const char *names[] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
        String s = parts->at(2);
        for (; month < 12; ++month) {
            if (s == names[month]) break;
        }
        if (month > 11) return 0;
        ++month;
    }
    int year = 0;
    {
        String s = parts->at(3);
        for (int i = 0; i < s->count(); ++i) {
            char ch = s->at(i);
            year *= 10;
            if ('0' <= ch && ch <= '9') year += ch - '0';
            else return 0;
        }
    }
    int hour = 0, minutes = 0, seconds = 0;
    {
        String s = parts->at(4);
        int *p = &hour;
        for (int i = 0; i < s->count(); ++i) {
            char ch = s->at(i);
            if (ch == ':') {
                if (p == &hour) p = &minutes;
                else if (p == &minutes) p = &seconds;
                else return 0;
            }
            else {
                *p *= 10;
                if ('0' <= ch && ch <= '9') *p += ch - '0';
                else return 0;
            }
        }
    }
    if (ok) *ok = true;
    return Date::compose(year, month, day, hour, minutes, seconds);
}

}} // namespace cc::http
