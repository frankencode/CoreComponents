/*
 * Stack.hpp -- fixed size stack
 *
 * Copyright (c) 2007-2009, Frank Mertens
 *
 * See ../LICENSE for the license.
 */
#ifndef PONA_STACK_HPP
#define PONA_STACK_HPP

#include "atoms"

namespace pona
{

template<class T>
class Stack: public Instance, public NonCopyable
{
public:
	Stack(int size)
		: fill_(0),
		  size_(size),
		  bufOwner_(true),
		  buf_(new T[size])
	{}

	Stack(T* buf, int size)
		: fill_(0),
		  size_(size),
		  bufOwner_(false),
		  buf_(buf)
	{}
	
	~Stack()
	{
		if (bufOwner_)
		{
			delete[] buf_;
			buf_ = 0;
		}
	}

	inline int size() const { return size_; }
	inline int fill() const { return fill_; }

	inline bool isFull() const { return fill_ == size_; }
	inline bool isEmpty() const { return fill_ == 0; }

	inline void push(T x)
	{
		assert(fill_ < size_);
		buf_[fill_++] = x;
	}

	inline T pop()
	{
		assert(fill_ > 0);
		return buf_[--fill_];
	}

	inline void clear()
	{
		fill_ = 0;
	}

	inline T top(int i = 0) const
	{
		assert(i < fill_);
		return buf_[fill_-i-1];
	}

	inline T bottom(int i = 0) const
	{
		assert(i < fill_);
		return buf_[i];
	}

private:
	int fill_;
	int size_;
	bool bufOwner_;
	T* buf_;
};

} // namespace pona

#endif // PONA_STACK_HPP
