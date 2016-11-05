/*
 * Copyright (C) 2007-2016 Frank Mertens.
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
inline Iterator<Container> begin(Container *c) { return Iterator<Container>(c, 0); }

template<class Container>
inline Iterator<Container> end(Container *c) { return Iterator<Container>(c, c->count()); }

template<class Container>
inline Iterator<Container> begin(Ref<Container> &c) { return Iterator<Container>(c, 0); }

template<class Container>
inline Iterator<Container> end(Ref<Container> &c) { return Iterator<Container>(c, c->count()); }

} // namespace cc
