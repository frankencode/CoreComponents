/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/String>
#include <cc/Variant>
#include <cc/str>

namespace cc {

class FormatSignal {};

extern FormatSignal nl;
extern FormatSignal flush;

class Stream;
template<class T> class Queue;

/** \brief Text formatting helper
  */
class Format: public Ref<StringList>
{
public:
    typedef Ref<StringList> Super;

    Format(String pattern, Stream *stream = 0);
    Format(Stream *stream = 0);
    ~Format();

    Format(const Format &b);

    Format &operator=(const Format &b);
    Format &operator<<(const String &s);
    Format &operator<<(const FormatSignal &s);

    inline Format &operator<<(const ByteArray *s) {
        if (isNull_) return *this;
        return *this << String(s);
    }

    template<class T>
    inline Format &operator<<(const T &x) {
        if (isNull_) return *this;
        return *this << str(x);
    }

    inline Format &operator<<(const Ref<StringList> &x) {
        if (isNull_) return *this;
        return *this << x->join("");
    }

private:
    void flush();

    Ref<Stream> stream_;
    bool isNull_;
    Ref< Queue<int> > placeHolder_;
};

/** \brief Zero storage formatting output sink
  */
class NullFormat {
public:
    template<class T>
    inline const NullFormat &operator<<(T) const { return *this; }
};

} // namespace cc
