/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Format>

namespace cc {

NewLine nl;
Flush flush;

Format::Format(const String &pattern, const Stream &stream):
    me{stream, stream && stream.isDiscarding()}
{
    if (me().discardMode) return;
    String p = pattern;
    long i = 0, n = 0;
    while (true) {
        long i0 = i;
        bool found = p.find("%%", &i);
        if (i0 < i) (*this).append(p.select(i0, i));
        if (!found) break;
        long j = (*this).count() + n;
        me().placeHolder.pushBack(j);
        ++n;
        i += 2;
    }
    me().injectionMode = me().placeHolder.count() > 0;
}

Format::Format(const Stream &stream):
    me{stream, stream && stream.isDiscarding()}
{}

Format::~Format()
{
    try {
        flush();
    }
    catch (...)
    {}
}

void Format::flush()
{
    if (me().discardMode) return;
    if (me().stream && (*this).count() > 0) {
        if (me().injectionMode && me().lastInsert.count() != 0) {
            while (me().placeHolder.count() > 0) {
                int j = 0;
                me().placeHolder.popFront(&j);
                (*this).insertAt(j, me().lastInsert);
            }
        }
        me().stream.write(String{*this});
        (*this).deplete();
    }
}

Format &Format::operator<<(const Bytes &s)
{
    if (me().discardMode) return *this;

    int j = (*this).count();

    if (me().injectionMode) {
        if (me().placeHolder.count() > 0) {
            me().placeHolder.popFront(&j);
            me().lastPosition = j;
        }
        else {
            j = ++me().lastPosition;
        }
        (*this).insertAt(j, s);
        me().lastInsert = s;
    }
    else {
        (*this).append(s);
    }

    return *this;
}

Format &Format::operator<<(const NewLine &)
{
    if (me().discardMode) return *this;
    (*this).append(String{"\n", 1});
    return *this;
}

Format &Format::operator<<(const Flush &)
{
    if (me().discardMode) return *this;
    flush();
    return *this;
}

} // namespace cc
