/*
 * generics.hpp -- generic nuclei
 *
 * Copyright (c) 2007-2010, Frank Mertens
 *
 * See ../LICENSE for the license.
 */
#ifndef PONA_GENERICS_HPP
#define PONA_GENERICS_HPP

#include "Instance.hpp"
#include "Ref.hpp"
#include "Exception.hpp"

namespace pona
{

/** Wrapper class to allow taking reference to classes not inherited
  * from pona::Instance. Please note, that it is impossible to reference
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

PONA_EXCEPTION(StreamException, Exception);
PONA_EXCEPTION(StreamSemanticException, StreamException);
PONA_EXCEPTION(StreamIoException, StreamException);
PONA_EXCEPTION(StreamEncodingException, StreamIoException);

class Stream: public Instance
{
public:
	virtual ~Stream() {}
	
	virtual bool isOpen() const = 0;
	virtual void close() = 0;
	
	virtual int readAvail(void* buf, int bufCapa) = 0;
	virtual void write(const void* buf, int bufFill) = 0;
	
	void read(void* buf, int bufCapa);
};

} // namespace pona

#endif // PONA_GENERICS_HPP
