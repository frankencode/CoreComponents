 /*
  * Copyright (C) 2007-2013 Frank Mertens.
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version
  * 2 of the License, or (at your option) any later version.
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

	inline static Ref<CircularBuffer, Owner> create(int size) {
		return new CircularBuffer(size);
	}
	inline static Ref<CircularBuffer, Owner> create(T *buf, int size) {
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
	inline T &at(int i) const { return front(i); }
	inline T get(int i) const { return front(i); }

	inline void push(const T &item) { pushBack(item); }
	inline void pop(T *item) { popFront(item); }
	inline T pop() { T item; popFront(&item); return item; }

	inline void clear()
	{
		fill_ = 0;
		head_ = size_ - 1;
		tail_ = size_ - 1;
	}

	inline void pushBack(const T &item)
	{
		FTL_ASSERT(fill_ != size_);
		++head_;
		if (head_ >= size_) head_ = 0;
		++fill_;
		buf_[head_] = item;
	}

	inline void popFront(T *item)
	{
		FTL_ASSERT(fill_ > 0);
		++tail_;
		if (tail_ >= size_) tail_ = 0;
		--fill_;
		*item = buf_[tail_];
	}

	inline void pushFront(const T &item)
	{
		FTL_ASSERT(fill_ < size_);
		buf_[tail_] = item;
		--tail_;
		if (tail_ < 0) tail_ = size_ - 1;
		++fill_;
	}

	inline void popBack(T *item)
	{
		FTL_ASSERT(fill_ > 0);
		*item = buf_[head_];
		--head_;
		if (head_ < 0) head_ = size_ - 1;
		--fill_;
	}

	inline T popFront() { T item; popFront(item); return item; }
	inline T popBack() { T item; popBack(item); return item; }

	inline T &back(int i = 0) const
	{
		FTL_ASSERT((0 <= i) && (i < fill_));
		i = -i;
		i += head_;
		if (i < 0) i += size_;
		return buf_[i];
	}

	inline T &front(int i = 0) const
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

	CircularBuffer(T *buf, int size)
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
	T *buf_;
};

} // namespace ftl

#endif // FTL_CIRCULARBUFFER_HPP
