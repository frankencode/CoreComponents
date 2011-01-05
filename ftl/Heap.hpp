/*
 * Heap.hpp -- static min/max heaps
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../COPYING for the license.
 */
#ifndef FTL_HEAP_HPP
#define FTL_HEAP_HPP

#include "containers.hpp"

namespace ftl
{

template<class T, template<class T> class Order = Ascending>
class GenericHeap: public Container< T, GenericHeap<T, Order> >, public Order<T>
{
public:
	typedef T Item;
	
	GenericHeap(int size)
		: fill_(0),
		  size_(size),
		  bufOwner_(true),
		  buf_(new T[size])
	{}
	
	GenericHeap(T* buf, int size)
		: fill_(0),
		  size_(size),
		  bufOwner_(false),
		  buf_(buf)
	{}
	
	~GenericHeap()
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

	inline GenericHeap& push(const T& item)
	{
		check(fill_ < size_);
		buf_[fill_] = item;
		++fill_;
		passUpLast();
		return *this;
	}

	inline GenericHeap& pop(T& item)
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
	
	inline T top() { check(!isEmpty()); return buf_[0]; }
	
	inline void clear() { fill_ = 0; }
	
protected:
	int fill_;    // current number of elements
	int size_;    // maximal number of elements
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
		int h = below(buf_[i], buf_[j]) ? i : j;
		return below(buf_[h], buf_[k]) ? h : k;
	}
	
	void passUpLast()
	{
		if (fill_ == 1) return;
		int i = fill_ - 1;
		while (i != 0) {
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
		while (true) {
			int j, lc, rc;
			lc = leftChild(i);
			rc = rightChild(i);
			
			if (rc < fill_) {
				j = min(i, lc, rc);
				if (j == i) break;
				
				xchg(i, j);
				i = j;
			}
			else if (lc < fill_) {
				if (below(buf_[lc], buf_[i])) xchg(i, lc);
				break;
			}
			else
				break;
		}
	}
};

template<class T>
class Heap: public GenericHeap<T, FlexibleSortOrder>
{
public:
	typedef GenericHeap<T, FlexibleSortOrder> Super;
	
	Heap(int size, int order = SortOrder::Ascending)
		: GenericHeap<T, FlexibleSortOrder>(size)
	{
		Super::setSortOrder(order);
	}
	Heap(T* buf, int size, int order = SortOrder::Ascending)
		: GenericHeap<T, FlexibleSortOrder>(buf, size)
	{
		Super::setSortOrder(order);
	}
	void reset(int order) {
		Super::clear();
		Super::setSortOrder(order);
	}
};

template<class T>
class MinHeap: public GenericHeap<T, Ascending>
{
public:
	MinHeap(int size): GenericHeap<T, Ascending>(size) {}
	MinHeap(T* buf, int size): GenericHeap<T, Ascending>(buf, size) {}
};

template<class T>
class MaxHeap: public GenericHeap<T, Descending>
{
public:
	MaxHeap(int size): GenericHeap<T, Descending>(size) {}
	MaxHeap(T* buf, int size): GenericHeap<T, Descending>(buf, size) {}
};

} // namespace ftl

#endif // FTL_HEAP_H
