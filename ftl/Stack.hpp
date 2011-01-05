/*
 * Stack.hpp -- static stack
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../COPYING for the license.
 */
#ifndef FTL_STACK_HPP
#define FTL_STACK_HPP

#include "containers.hpp"

namespace ftl
{

template<class T>
class Stack: public Container< T, Stack<T> >, public Sequence<T, int>
{
public:
	typedef int Index;
	typedef T Item;
	
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
	
	inline int first() const { return 0; }
	inline int last() const { return fill_ - 1; }
	inline int end() const { return fill_; }
	
	inline bool def(int i) const { return (0 <= i) && (i < fill_); }
	inline T& at(int i) const { return bottom(i); }
	inline T get(int i) const { return bottom(i); }
	
	inline Stack& push(const T& item) {
		check(fill_ < size_);
		buf_[fill_++] = item;
		return *this;
	}
	inline Stack& pop(T& item)
	{
		check(fill_ > 0);
		item = buf_[--fill_];
		return *this;
	}
	inline T pop() {
		T item;
		pop(item);
		return item;
	}
	
	inline Stack& popMore(int n) {
		check(fill_ >= n);
		fill_ -= n;
		return *this;
	}
	
	inline void clear() { fill_ = 0; }
	
	inline T& top(int i = 0) const {
		check(i < fill_);
		return buf_[fill_-i-1];
	}
	
	inline T& bottom(int i = 0) const {
		check(i < fill_);
		return buf_[i];
	}
	
	inline T* data() const { return buf_; }
	inline operator T*() const { return buf_; }
	
private:
	int fill_;
	int size_;
	bool bufOwner_;
	T* buf_;
};

} // namespace ftl

#endif // FTL_STACK_HPP
