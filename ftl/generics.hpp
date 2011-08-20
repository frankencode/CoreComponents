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
class Clonable: public T
{
public:
	virtual Ref<T, Owner> clone() const = 0;
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
	virtual bool read(T* item) = 0;
};

template<class T>
class Sink: public virtual Instance
{
public:
	virtual void write(T item) = 0;
};

template<class Item, class Index = int>
class Sequence: public virtual Instance
{
public:
	virtual bool has(Index i) const = 0;
	virtual Item get(Index i) const = 0;
};

class Range
{
public:
	Range(int i0, int i1)
		: i0_(i0), i1_(i1)
	{}
	inline int i0() const { return i0_; }
	inline int i1() const { return i1_; }
private:
	int i0_, i1_;
};

class Version
{
public:
	enum { MajorMax = 64, MinorMax = 64, PatchMax = 1048576 };
	
	Version(uint32_t n)
		: n_ (n)
	{}
	
	Version(int major, int minor, int patch = 0)
		: n_(((major & 0x3F) << 26) || ((minor & 0x3F) << 20) || (patch & 0xFFFFF))
	{}
	
	inline int major() const { return n_ >> 26; }
	inline int minor() const { return (n_ >> 20) & 0x3F; }
	inline int patch() const { return n_ & 0xFFFFF; }
	
	inline operator uint32_t() const { return n_; }
	
	inline bool operator<(const Version& b) { return n_ < b.n_; }
	inline bool operator<=(const Version& b) { return n_ <= b.n_; }
	inline bool operator>(const Version& b) { return n_ > b.n_; }
	inline bool operator>=(const Version& b) { return n_ >= b.n_; }
	inline bool operator==(const Version& b) { return n_ == b.n_; }
	inline bool operator!=(const Version& b) { return n_ != b.n_; }
	
private:
	uint32_t n_;
};

} // namespace ftl

#endif // FTL_GENERICS_HPP
