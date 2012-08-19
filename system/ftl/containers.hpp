/*
 * containers.hpp -- generic components and interfaces for containers
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
 */
#ifndef FTL_CONTAINERS_HPP
#define FTL_CONTAINERS_HPP

#include "atoms"

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

	inline CT &operator<<(const T &item) { push(item); return *static_cast<CT*>(this); }
	inline CT &operator>>(T &item) { pop(&item); return *static_cast<CT*>(this); }
};

template<
	class T1,
	class T2,
	template<class> class CT1,
	template<class> class GASP1,
	template<class> class CT2,
	template<class> class GASP2
>
inline Ref< CT1<T1>, GASP1 > operator<<(Ref< CT1<T1>, GASP1 > a, Ref< CT2<T1>, GASP2 > b) {
	for (T2 item; b->read(&item); a->push(item));
	return a;
}

template<
	class T1,
	class T2,
	template<class> class CT1,
	template<class> class GASP1,
	template<class> class CT2,
	template<class> class GASP2
>
inline Ref< CT2<T2>, GASP2 > operator>>(Ref< CT1<T1>, GASP1 > a, Ref< CT2<T2>, GASP2 > b) {
	for (T1 item; a->read(&item); b->push(item));
	return b;
}

template<class Container>
class GenericIterator: public Source<typename Container::Item>
{
public:
	typedef typename Container::Item Item;

	inline static Ref<GenericIterator, Owner> newInstance(Ref<Container> container) {
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
	GenericIterator(Ref<Container> container)
		: container_(container),
		  i_(0)
	{}

	Ref<Container, Owner> container_;
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
