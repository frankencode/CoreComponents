/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FKIT_CONTAINERS_H
#define FKIT_CONTAINERS_H

#include "generics.h"

namespace fkit
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

template<class T, class CT>
class Container: public Source<T>, public NonCopyable
{
public:
	virtual bool isEmpty() const = 0;
	virtual T pop() = 0;

	inline bool read(T *item) {
		if (!isEmpty()) {
			pop(item);
			return true;
		}
		return false;
	}

	virtual void push(const T &item) = 0;
	virtual void pop(T *item) = 0;

	inline CT &operator<<(const T &item) { push(item); return *static_cast<CT *>(this); }
	inline CT &operator>>(T &item) { pop(&item); return *static_cast<CT *>(this); }
};

template<class Container>
class GenericIterator: public Source<typename Container::Item>
{
public:
	typedef typename Container::Item Item;

	inline static Ref<GenericIterator> create(Container *container) {
		return new GenericIterator(container);
	}

	inline bool read(Item *item) {
		bool more = container_->has(i_);
		if (more)
			*item = container_->get(i_++);
		return more;
	}

	inline int currentIndex() const { return i_; }

private:
	GenericIterator(Container *container)
		: container_(container),
		  i_(0)
	{}

	Ref<Container> container_;
	int i_;
};

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

} // namespace fkit

#endif // FKIT_CONTAINERS_H
