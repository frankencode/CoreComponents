/*
 * Stack.hpp -- static stack
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
 */
#ifndef FTL_STACK_HPP
#define FTL_STACK_HPP

#include "containers.hpp"

namespace ftl
{

template<class T>
class Stack: public Container< T, Stack<T> >
{
public:
	typedef int Index;
	typedef T Item;

	inline static Ref<Stack, Owner> newInstance(int size) {
		return new Stack(size);
	}
	inline static Ref<Stack, Owner> newInstance(T* buf, int size) {
		return new Stack(buf, size);
	}

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
	inline int length() const { return fill_; }
	inline bool isFull() const { return fill_ == size_; }
	inline bool isEmpty() const { return fill_ == 0; }

	inline bool has(int i) const { return (0 <= i) && (i < fill_); }
	inline T& at(int i) const { return bottom(i); }
	inline T get(int i) const { return bottom(i); }

	inline Stack& push(const T& item) {
		FTL_ASSERT(fill_ < size_);
		buf_[fill_++] = item;
		return *this;
	}
	inline Stack& pop(T* item)
	{
		FTL_ASSERT(fill_ > 0);
		*item = buf_[--fill_];
		return *this;
	}
	inline T pop() {
		T item;
		pop(&item);
		return item;
	}

	inline Stack& popMore(int n) {
		FTL_ASSERT(fill_ >= n);
		fill_ -= n;
		return *this;
	}

	inline void clear() { fill_ = 0; }

	inline T& top(int i = 0) const {
		FTL_ASSERT(i < fill_);
		return buf_[fill_-i-1];
	}

	inline T& bottom(int i = 0) const {
		FTL_ASSERT(i < fill_);
		return buf_[i];
	}

	inline T* data() const { return buf_; }
	inline operator T*() const { return buf_; }

private:
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

	int fill_;
	int size_;
	bool bufOwner_;
	T* buf_;
};

} // namespace ftl

#endif // FTL_STACK_HPP
