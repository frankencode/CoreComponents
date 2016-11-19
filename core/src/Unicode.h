/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/ByteArray>
#include <cc/Utf8Walker>

namespace cc {

/** \class Unicode Unicode.h cc/Unicode
  * \ingroup unicode
  * \brief Seemless access to %Unicode characters of an UTF-8 encoded string
  * \see String
  */
class Unicode: public Object
{
public:
    typedef uchar_t Item;

    inline static Ref<Unicode> open(const ByteArray *data)
    {
        return new Unicode(data);
    }

    inline bool has(int i) const {
        walkTo(i);
        return walker_.valid();
    }

    inline uchar_t at(int i) const {
        walkTo(i);
        return walker_.getChar();
    }

    inline int count() const {
        if (n_ == -1) {
            if (!walker_.valid()) {
                walker_ = Utf8Walker(walker_.data());
                i_ = 0;
            }
            while (walker_.valid()) { ++walker_; ++i_; }
            n_ = i_;
            if (i_ > 0) { --walker_; --i_; }
        }
        return n_;
    }

    inline int offset() const {
        return walker_.pos() - walker_.data();
    }

    Ref<ByteArray> copy(int i0, int i1) const;
    inline Ref<ByteArray> head(int n) const { return copy(0, n); }
    inline Ref<ByteArray> tail(int n) const { return copy(count() - n, n); }

    inline int index(const char *pos) const {
        if (!walker_.valid()) {
            walker_ = Utf8Walker(walker_.data());
            i_ = 0;
        }
        while (walker_.pos() < pos) { ++walker_; ++i_; }
        while (walker_.pos() > pos) { --walker_; --i_; }
        return i_;
    }

private:
    Unicode(const ByteArray *data):
        data_(data),
        walker_(data->chars()),
        i_(0), n_(-1)
    {}

    inline void walkTo(int i) const {
        if (!walker_.valid()) {
            walker_ = Utf8Walker(walker_.data());
            i_ = 0;
        }
        while (i_ < i) { ++walker_; ++i_; }
        while (i_ > i) { --walker_; --i_; }
    }

    Ref<const ByteArray> data_;
    mutable Utf8Walker walker_;
    mutable int i_, n_;
};

template<>
class Iterator<Unicode> {
public:
    Iterator(Unicode *c, int i):
        c_(c),
        i_(i)
    {}

    inline uchar_t operator*() const { return c_->at(i_); }
    inline Iterator &operator++() { ++i_; return *this; }
    inline bool operator!=(const Iterator &b) const { return i_ != b.i_; }

private:
    Unicode *c_;
    int i_;
};

} // namespace cc
