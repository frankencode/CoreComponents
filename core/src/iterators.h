/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

namespace cc {

template<class Container>
class Iterator {
public:
    Iterator(Container *c, int i):
        c_(c),
        i_(i)
    {}

    inline typename Container::Item &operator*() const { return c_->at(i_); }
    inline Iterator &operator++() { ++i_; return *this; }
    inline bool operator!=(const Iterator &b) const { return i_ != b.i_; }

private:
    Container *c_;
    int i_;
};

template<class Container>
class ConstIterator {
public:
    ConstIterator(const Container *c, int i):
        c_(c),
        i_(i)
    {}

    inline typename Container::Item operator*() const { return c_->at(i_); }
    inline ConstIterator &operator++() { ++i_; return *this; }
    inline bool operator!=(const ConstIterator &b) const { return i_ != b.i_; }

private:
    const Container *c_;
    int i_;
};

template<class Container>
inline typename Container::const_iterator begin(const Container *c) { return c->begin(); }

template<class Container>
inline typename Container::const_iterator end(const Container *c) { return c->end(); }

template<class Container>
inline typename Container::iterator begin(Container *c) { return c->begin(); }

template<class Container>
inline typename Container::iterator end(Container *c) { return c->end(); }

template<class Container>
inline typename Container::const_iterator begin(const Ref<const Container> &c) { return c->begin(); }

template<class Container>
inline typename Container::const_iterator end(const Ref<const Container> &c) { return c->end(); }

template<class Container>
inline typename Container::iterator begin(Ref<Container> c) { return c->begin(); }

template<class Container>
inline typename Container::iterator end(Ref<Container> c) { return c->end(); }

} // namespace cc
