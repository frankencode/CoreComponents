/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUX_CONTAINERS_H
#define FLUX_CONTAINERS_H

#include "generics.h"

namespace flux
{

namespace container
{

template<class C>
bool compare(const C &a, const C &b)
{
	typedef typename C::Item T;
	int n = a.size() > b.size() ? a.size() : b.size();
	for (int i = 0; i < n; ++i) {
		T x = (i < a.size()) ? a.at(i) : T();
		T y = (i < b.size()) ? b.at(i) : T();
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
