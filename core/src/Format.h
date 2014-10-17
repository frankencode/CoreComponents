/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUX_FORMAT_H
#define FLUX_FORMAT_H

#include <flux/String>
#include <flux/Variant>
#include <flux/str>

namespace flux {

class FormatSignal {};

extern FormatSignal nl;
extern FormatSignal flush;

class Stream;
template<class T> class Queue;

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

class NullFormat {
public:
    template<class T>
    inline const NullFormat &operator<<(T) const { return *this; }
};

} // namespace flux

#endif // FLUX_FORMAT_H
