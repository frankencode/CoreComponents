/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUX_CONTAINERS_H
#define FLUX_CONTAINERS_H

#include <flux/generics>

namespace flux {

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

template<class T>
class Ascending {
public:
    static inline bool below(const T &a, const T &b) { return a < b; }
};

template<class T>
class Descending {
public:
    static inline bool below(const T &a, const T &b) { return b < a; }
};

class SortOrder {
public:
    enum { Desc = 0, Asc = 1, Descending = Desc, Ascending = Asc };
};

template<class T>
class FlexibleSortOrder: public SortOrder {
public:
    FlexibleSortOrder(int order = Asc): asc_(order == Asc) {}
    inline bool below(const T &a, const T &b) { return (a < b) == asc_; }
protected:
    void setSortOrder(int order) { asc_ = (order == Asc); }
private:
    bool asc_;
};

} // namespace flux

#endif // FLUX_CONTAINERS_H
