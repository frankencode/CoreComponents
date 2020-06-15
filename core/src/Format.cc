/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Format>
#include <cc/NullStream>

namespace cc {

FormatSignal nl;
FormatSignal flush;

Format::Format(const String &pattern, Stream *stream):
    stream_{stream},
    isNull_{stream && NullStream::instance() ? stream == NullStream::instance() : false},
    lastPosition_{0}
{
    if (isNull_) return;
    int i0 = 0, n = 0;
    while (true) {
        int i1 = pattern->scan("%%", i0);
        if (i0 < i1)
            (*this)->append(pattern->copy(i0, i1));
        if (i1 == pattern->count()) break;
        int j = (*this)->count() + n;
        if (!placeHolder_) placeHolder_ = QueueInstance<int>::create();
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
    if (isNull_) return;
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
    if (stream_ && (*this)->count() > 0) {
        if (placeHolder_ && lastInsert_ != "") {
            while (placeHolder_->count() > 0) {
                int j = placeHolder_->pop();
                (*this)->insertAt(j, lastInsert_);
            }
        }
        stream_->write(*this);
        (*this)->deplete();
    }
}

Format &Format::operator<<(const String &s)
{
    if (isNull_) return *this;
    int j = (*this)->count();
    if (placeHolder_) {
        if (placeHolder_->count() > 0) {
            j = placeHolder_->pop();
            lastPosition_ = j;
        }
        else j = ++lastPosition_;
    }
    (*this)->insertAt(j, s);
    lastInsert_ = s;
    return *this;
}

Format &Format::operator<<(const FormatSignal &s)
{
    if (isNull_) return *this;
    if (&s == &cc::nl) operator<<(String{"\n"});
    else if (&s == &cc::flush) Format::flush();
    return *this;
}

} // namesp
