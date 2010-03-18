/*
 * Heap.hpp -- static min/max heap
 *
 * Copyright (c) 2007-2010, Frank Mertens
 *
 * See ../LICENSE for the license.
 */
#ifndef PONA_HEAP_HPP
#define PONA_HEAP_HPP

#include "atoms"
#include "Exception.hpp"

namespace pona
{

template<class T, template<class T> class Order = Ascending>
class Heap: public Instance, private Order<T>, public NonCopyable
{
public:
	typedef T Item;
	
	Heap(int size)
		: fill_(0),
		  size_(size),
		  bufOwner_(true),
		  buf_(new T[size])
	{}
	
	Heap(T* buf, int size)
		: fill_(0),
		  size_(size),
		  bufOwner_(false),
		  buf_(buf)
	{}
	
	~Heap()
	{
		if (bufOwner_)
		{
			delete[] buf_;
			buf_ = 0;
		}
	}
	
	inline int size() const { return size_; }
	inline int fill() const { return fill_; }
	inline bool full() const { return fill_ == size_; }
	inline bool empty() const { return fill_ == 0; }

	inline Heap& push(const T& item)
	{
		check(fill_ < size_);
		buf_[fill_] = item;
		++fill_;
		passUpLast();
		return *this;
	}

	inline Heap& pop(T& item)
	{
		check(fill_ > 0);
		item = buf_[0];
		--fill_;
		buf_[0] = buf_[fill_];
		passDownFromTop();
		return *this;
	}
	
	inline T pop() {
		T item;
		pop(item);
		return item;
	}
	
	inline T top() { check(!empty()); return buf_[0]; }
	
	inline Heap& operator<<(const T& item) { return push(item); }
	inline Heap& operator>>(T& item) { return pop(item); }
	
	template<template<class> class CB>
	inline Heap& operator<<(CB<T>& cb) {
		while (!cb.empty()) {
			T item;
			cb >> item;
			*this << item;
		}
		return *this;
	}
	
	template<template<class> class CB>
	inline Heap& operator>>(CB<T>& cb) {
		while (!empty()) {
			T item;
			*this >> item;
			cb << item;
		}
		return *this;
	}
	
	inline void clear() { fill_ = 0; }
	
protected:
	int fill_;    // current number of elements
	int size_;    // maximal number of elements
	bool ascOrder_;    // used for ascending or descending sort?
	bool bufOwner_;
	T* buf_;    // memory buffer used for storing elements
	
	inline static int parent(int i) { return (i - 1) / 2; }
	inline static int leftChild(int i) { return 2 * i + 1; }
	inline static int rightChild(int i) { return 2 * i + 2; }

	void xchg(int i, int j)
	{
		T h = buf_[i];
		buf_[i] = buf_[j];
		buf_[j] = h;
	}

	int min(int i, int j, int k)
	{
		if (below(buf_[i], buf_[j]) && below(buf_[i], buf_[k])) return i;
		if (below(buf_[j], buf_[i]) && below(buf_[j], buf_[k])) return j;
		return k;
	}
	
	/*int max(int i, int j, int k)
	{
		if (below(buf_[j], buf_[i]) && below(buf_[k], buf_[i])) return i;
		if (below(buf_[k], buf_[j]) && below(buf_[i], buf_[j])) return j;
		return k;
	}*/
	
	void passUpLast()
	{
		if (fill_ == 1) return;
		
		int i = fill_ - 1;
		
		while (i != 0)
		{
			int j;
			j = parent(i);
			if (below(buf_[j], buf_[i])) break;
			xchg(i, j);
			i = j;
		}
	}
	
	void passDownFromTop()
	{
		if (fill_ == 0) return;
		
		int i = 0;
		
		while (true)
		{
			int j, lc, rc;
			lc = leftChild(i);
			rc = rightChild(i);
		
			if ((lc < fill_) && ((rc < fill_)))
			{
				j = /*(ascOrder_) ?*/ min(i, lc, rc) /*: max(i, lc, rc)*/;
				if (j == i) break;
				
				xchg(i, j);
				i = j;
				
				continue;
			}
			
			if (lc < fill_)
			{
				if (below(buf_[i], buf_[lc])) xchg(i, lc);
			}
			else
			if (rc < fill_)
			{
				if (below(buf_[i], buf_[rc])) xchg(i, rc);
			}
			
			break;
		}
	}
};

} // namespace pona

#endif // PONA_HEAP_HPP
