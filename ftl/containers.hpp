/*
 * containers.hpp -- generic components and interfaces for containers
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../COPYING for the license.
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
	inline bool hasNext() { return !isEmpty(); }
	inline T next() { return pop(); }
	
	virtual CT& push(const T& item) = 0;
	virtual CT& pop(T& item) = 0;
	inline CT& operator<<(const T& item) { return push(item); }
	inline CT& operator>>(T& item) { return pop(item); }
	inline CT& operator<<(Source<T>& source) {
		while (source.hasNext())
			push(source.next());
		return *static_cast<CT*>(this);
	}
};

template<class Container>
class GenericIterator: public Source<typename Container::Item>
{
public:
	typedef typename Container::Item Item;
	typedef typename Container::Index Index;
	
	GenericIterator(Ref<Container> container)
		: container_(container),
		  i_(container->first())
	{}
	
	inline bool hasNext() { return container_->def(i_); }
	inline bool hasPrevious() { return container_->def(i_ - 1); }
	inline Item next() { return container_->at(i_++); }
	inline Item previous() { return container_->at(--i_); }
	
	inline void add(const Item& item) { container_->insert(i_, item); }
	inline void remove() { container_->remove(i_ - 1); }
	inline void set(const Item& item) { container_->set(i_ - 1, item); }
	
	inline operator Index() const { return i_; }
	
private:
	Ref<Container> container_;
	typename Container::Index i_;
};

template<
	class T,
	template<class T> class Source,
	template<class T> class Target
>
inline Target<T>& operator>>(Source<T>& source, Target<T>& target)
{
	while (!source.isEmpty())
		target.push(source.pop());
	return target;
}

template<class T>
class Ascending {
public:
	static inline bool below(const T& a, const T& b) { return a < b; }
};

template<class T>
class Descending {
public:
	static inline bool below(const T& a, const T& b) { return b < a; }
};

class SortOrder {
public:
	enum { Desc = 0, Asc = 1, Descending = Desc, Ascending = Asc };
};

template<class T>
class FlexibleSortOrder: public SortOrder {
public:
	FlexibleSortOrder(int order = Asc): asc_(order == Asc) {}
	inline bool below(const T& a, const T& b) { return (a < b) == asc_; }
protected:
	void setSortOrder(int order) { asc_ = (order == Asc); }
private:
	bool asc_;
};

} // namespace ftl

#endif // FTL_CONTAINERS_HPP
