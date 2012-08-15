/*
 * CircularBuffer.hpp -- a fixed size queue
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
 */
#ifndef FTL_CIRCULARBUFFER_HPP
#define FTL_CIRCULARBUFFER_HPP

#include "containers.hpp"

namespace ftl
{

template<class T>
class CircularBuffer: public Container< T, CircularBuffer<T> >, public Sequence<T, int>
{
public:
	typedef int Index;
	typedef T Item;

	inline static Ref<CircularBuffer, Owner> newInstance(int size) {
		return new CircularBuffer(size);
	}
	inline static Ref<CircularBuffer, Owner> newInstance(T* buf, int size) {
		return new CircularBuffer(buf, size);
	}

	~CircularBuffer()
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
	inline T& at(int i) const { return front(i); }
	inline T get(int i) const { return front(i); }

	inline CircularBuffer& push(const T& item) { return pushBack(item); }
	inline CircularBuffer& pop(T* item) { return popFront(item); }
	inline T pop() { T item; popFront(&item); return item; }

	inline void clear()
	{
		fill_ = 0;
		head_ = size_ - 1;
		tail_ = size_ - 1;
	}

	inline CircularBuffer& pushBack(const T& item)
	{
		FTL_ASSERT(fill_ != size_);
		++head_;
		if (head_ >= size_) head_ = 0;
		++fill_;
		buf_[head_] = item;
		return *this;
	}

	inline CircularBuffer& popFront(T* item)
	{
		FTL_ASSERT(fill_ > 0);
		++tail_;
		if (tail_ >= size_) tail_ = 0;
		--fill_;
		*item = buf_[tail_];
		return *this;
	}

	inline CircularBuffer& pushFront(const T& item)
	{
		FTL_ASSERT(fill_ < size_);
		buf_[tail_] = item;
		--tail_;
		if (tail_ < 0) tail_ = size_ - 1;
		++fill_;
		return *this;
	}

	inline CircularBuffer& popBack(T* item)
	{
		FTL_ASSERT(fill_ > 0);
		*item = buf_[head_];
		--head_;
		if (head_ < 0) head_ = size_ - 1;
		--fill_;
		return *this;
	}

	inline T popFront() { T item; popFront(item); return item; }
	inline T popBack() { T item; popBack(item); return item; }

	inline T& back(int i = 0) const
	{
		FTL_ASSERT((0 <= i) && (i < fill_));
		i = -i;
		i += head_;
		if (i < 0) i += size_;
		return buf_[i];
	}

	inline T& front(int i = 0) const
	{
		FTL_ASSERT((0 <= i) && (i < fill_));
		i += tail_ + 1;
		if (i >= size_) i -= size_;
		return buf_[i];
	}

private:
	CircularBuffer(int size)
		: fill_(0),
		  size_(size),
		  head_(size_-1),
		  tail_(size_-1),
		  bufOwner_(true),
		  buf_(new T[size_])
	{}

	CircularBuffer(T* buf, int size)
		: fill_(0),
		  size_(size),
		  head_(size_-1),
		  tail_(size_-1),
		  bufOwner_(false),
		  buf_(buf)
	{}

	int fill_;
	int size_;
	int head_;
	int tail_;
	bool bufOwner_;
	T* buf_;
};

} // namespace ftl

#endif // FTL_CIRCULARBUFFER_HPP
