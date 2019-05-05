/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Queue>
#include <cc/NullStream>
#include <cc/Format>

namespace cc {

FormatSignal nl;
FormatSignal flush;

Format::Format(String pattern, Stream *stream):
    stream_{stream},
    isNull_{stream && NullStream::instance() ? stream == NullStream::instance() : false},
    lastPosition_{0}
{
    if (isNull_) return;
    set(StringList::create());
    int i0 = 0, n = 0;
    while (true) {
        int i1 = pattern->find("%%", i0);
        if (i0 < i1)
            get()->append(pattern->copy(i0, i1));
        if (i1 == pattern->count()) break;
        int j = get()->count() + n;
        if (!placeHolder_) placeHolder_ = Queue<int>::create();
        placeHolder_->push(j);
        ++n;
        i0 = i1 + 2;
    }
}

Format::Format(Stream *stream):
    stream_{stream},
    isNull_{stream && NullStream::instance() ? stream == NullStream::instance() : false},
    lastPosition_{0}
{
    set(StringList::create());
}

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
    if (isNull_) return;
    if (stream_ && get()->count() > 0) {
        if (placeHolder_ && lastInsert_ != "") {
            while (placeHolder_->count() > 0) {
                int j = placeHolder_->pop();
                get()->insertAt(j, lastInsert_);
            }
        }
        stream_->write(get());
        get()->deplete();
    }
}

Format::Format(const Format &b):
    Super{b.get()},
    stream_{b.stream_},
    isNull_{b.isNull_},
    placeHolder_{b.placeHolder_}
{}

Format &Format::operator=(const Format &b)
{
    set(b.get());
    stream_ = b.stream_;
    placeHolder_ = b.placeHolder_;
    isNull_ = b.isNull_;
    return *this;
}

Format &Format::operator<<(const String &s)
{
    if (isNull_) return *this;
    int j = get()->count();
    if (placeHolder_) {
        if (placeHolder_->count() > 0) {
            j = placeHolder_->pop();
            lastPosition_ = j;
        }
        else j = ++lastPosition_;
    }
    get()->insertAt(j, s);
    lastInsert_ = s;
    return *this;
}

Format &Format::operator<<(const FormatSignal &s)
{
    if (isNull_) return *this;
    if (&s == &cc::nl) operator<<(String("\n"));
    else if (&s == &cc::flush) Format::flush();
    return *this;
}

} // namespace cc
