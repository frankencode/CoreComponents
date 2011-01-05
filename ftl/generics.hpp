/*
 * generics.hpp -- generic nuclei
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../COPYING for the license.
 */
#ifndef FTL_GENERICS_HPP
#define FTL_GENERICS_HPP

#include "Instance.hpp"
#include "Ref.hpp"

namespace ftl
{

/** Wrapper class to allow taking reference to classes not inherited
  * from ftl::Instance. Please note, that it is impossible to reference
  * the same alien object by two different alien wrappers.
  */
template<class T>
class Alien: public Instance
{
public:
	typedef void (*DelFunc)(T*);

	Alien(T* guest, DelFunc delFunc = 0)
		: guest_(guest),
		  delFunc_(delFunc)
	{}
	
	~Alien()
	{
		if (guest_ != 0)
		{
			if (delFunc_) delFunc_(guest_);
			else delete guest_;
			guest_ = 0;
		}
	}
	
	inline T* get() const { return guest_; }
	
private:
	mutable T* guest_;
	DelFunc delFunc_;
};

class Action: public virtual Instance
{
public:
	virtual void run() = 0;
};

template<class T>
class Duplicable: public T
{
public:
	virtual Ref<T, Owner> duplicate() const = 0;
};

template<class T>
class Return: public T
{
public:
	Return(const T& b): T(b) {}
	inline operator T() { return *this; }
};

template<class T>
class Source: public virtual Instance
{
public:
	virtual bool hasNext() = 0;
	virtual T next() = 0;
};

template<class Item, class Index = int>
class Sequence: public virtual Instance
{
public:
	virtual bool def(Index i) const = 0;
	virtual Item get(Index i) const = 0;
};

} // namespace ftl

#endif // FTL_GENERICS_HPP
