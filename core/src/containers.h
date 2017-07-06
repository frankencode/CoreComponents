/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/generics>
#include <cc/iterators>

namespace cc {

namespace container {

template<class C>
bool compare(const C &a, const C &b)
{
    typedef typename C::Item T;
    int n = a.count() > b.count() ? a.count() : b.count();
    for (int i = 0; i < n; ++i) {
        T x = (i < a.count()) ? a.at(i) : T();
        T y = (i < b.count()) ? b.at(i) : T();
        if (x < y) return -1;
        if (y < x) return 1;
    }
    return 0;
}

} // namespace container

enum SortOrder { Desc = 0, Asc = 1, Descending = Desc, Ascending = Asc };

template<class T>
class AscendingSortOrder {
public:
    static inline bool below(const T &a, const T &b) { return a < b; }
};

template<class T>
class DescendingSortOrder {
public:
    static inline bool below(const T &a, const T &b) { return b < a; }
};

template<class T>
class FlexibleSortOrder {
public:
    FlexibleSortOrder(SortOrder order = Asc): asc_(order == Asc) {}
    inline bool below(const T &a, const T &b) { return (a < b) == asc_; }
protected:
    void setSortOrder(SortOrder order) { asc_ = (order == Asc); }
private:
    bool asc_;
};

} // namespace cc
