/*
 * Queue.hpp -- static queue
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../COPYING for the license.
 */
#ifndef FTL_QUEUE_HPP
#define FTL_QUEUE_HPP

#include "containers.hpp"

namespace ftl
{

template<class T>
class Queue: public Container< T, Queue<T> >, public Sequence<T, int>
{
public:
	typedef int Index;
	typedef T Item;
	
	Queue(int size)
		: fill_(0),
		  size_(size),
		  head_(size_-1),
		  tail_(size_-1),
		  bufOwner_(true),
		  buf_(new T[size_])
	{}
	
	Queue(T* buf, int size)
		: fill_(0),
		  size_(size),
		  head_(size_-1),
		  tail_(size_-1),
		  bufOwner_(false),
		  buf_(buf)
	{}
	
	~Queue()
	{
		if (bufOwner_)
		{
			delete[] buf_;
			buf_ = 0;
		}
	}
	
	inline int first() const { return (fill_ > 0) - 1; }
	inline int last() const { return fill_ - 1; }
	inline int end() const { return fill_; }
	
	inline int size() const { return size_; }
	inline int fill() const { return fill_; }
	inline bool isFull() const { return fill_ == size_; }
	inline bool isEmpty() const { return fill_ == 0; }
	
	inline bool def(int i) const { return (0 <= i) && (i < fill_); }
	inline T& at(int i) const { return front(i); }
	inline T get(int i) const { return front(i); }
	
	inline Queue& push(const T& item) { return pushBack(item); }
	inline Queue& pop(T& item) { return popFront(item); }
	inline T pop() { T item; popFront(item); return item; }
	
	inline void clear()
	{
		fill_ = 0;
		head_ = size_ - 1;
		tail_ = size_ - 1;
	}
	
	inline Queue& pushBack(const T& item)
	{
		check(fill_ != size_);
		++head_;
		if (head_ >= size_) head_ = 0;
		++fill_;
		buf_[head_] = item;
		return *this;
	}
	
	inline Queue& popFront(T& item)
	{
		check(fill_ > 0);
		++tail_;
		if (tail_ >= size_) tail_ = 0;
		--fill_;
		item = buf_[tail_];
		return *this;
	}
	
	inline Queue& pushFront(const T& item)
	{
		check(fill_ < size_);
		buf_[tail_] = item;
		--tail_;
		if (tail_ < 0) tail_ = size_ - 1;
		++fill_;
		return *this;
	}
	
	inline Queue& popBack(T& item)
	{
		check(fill_ > 0);
		item = buf_[head_];
		--head_;
		if (head_ < 0) head_ = size_ - 1;
		--fill_;
		return *this;
	}

	inline T popFront() { T item; popFront(item); return item; }
	inline T popBack() { T item; popBack(item); return item; }

	inline T& back(int i = 0) const
	{
		check((0 <= i) && (i < fill_));
		i = -i;
		i += head_;
		if (i < 0) i += size_;
		return buf_[i];
	}
	
	inline T& front(int i = 0) const
	{
		check((0 <= i) && (i < fill_));
		i += tail_ + 1;
		if (i >= size_) i -= size_;
		return buf_[i];
	}
	
private:
	int fill_;
	int size_;
	int head_;
	int tail_;
	bool bufOwner_;
	T* buf_;
};

} // namespace ftl

#endif // FTL_QUEUE_HPP
