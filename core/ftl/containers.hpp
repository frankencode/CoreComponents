 /*
  * Copyright (C) 2007-2013 Frank Mertens.
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version
  * 2 of the License, or (at your option) any later version.
  */
#ifndef FTL_CONTAINERS_HPP
#define FTL_CONTAINERS_HPP

#include "generics.hpp"

namespace ftl
{

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

	inline static hook<GenericIterator> create(Container *container) {
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

	hook<Container> container_;
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

} // namespace ftl

#endif // FTL_CONTAINERS_HPP
