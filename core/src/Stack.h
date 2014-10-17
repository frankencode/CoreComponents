/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUX_STACK_H
#define FLUX_STACK_H

#include <flux/containers>

namespace flux {

template<class T>
class Stack: public Object
{
public:
	typedef T Item;

	inline static Ref<Stack> create(int size) {
		return new Stack(size);
	}
	inline static Ref<Stack> wrap(T *buf, int size) {
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

	inline int maxCount() const { return size_; }
	inline int count() const { return fill_; }

	inline bool has(int i) const { return (0 <= i) && (i < fill_); }
	inline T &at(int i) const { return bottom(i); }
	inline T get(int i) const { return bottom(i); }

	inline void push(const T &item) {
		FLUX_ASSERT(fill_ < size_);
		buf_[fill_++] = item;
	}
	inline void pop(T *item) {
		FLUX_ASSERT(fill_ > 0);
		*item = buf_[--fill_];
	}
	inline T pop() {
		T item;
		pop(&item);
		return item;
	}

	inline Stack &popMore(int n) {
		FLUX_ASSERT(fill_ >= n);
		fill_ -= n;
		return *this;
	}

	inline void clear() { fill_ = 0; }

	inline T &top(int i = 0) const {
		FLUX_ASSERT(i < fill_);
		return buf_[fill_-i-1];
	}

	inline T &bottom(int i = 0) const {
		FLUX_ASSERT(i < fill_);
		return buf_[i];
	}

	inline T *data() const { return buf_; }
	inline operator T*() const { return buf_; }

private:
	Stack(int size)
		: fill_(0),
		  size_(size),
		  bufOwner_(true),
		  buf_(new T[size])
	{}

	Stack(T *buf, int size)
		: fill_(0),
		  size_(size),
		  bufOwner_(false),
		  buf_(buf)
	{}

	int fill_;
	int size_;
	bool bufOwner_;
	T *buf_;
};

} // namespace flux

#endif // FLUX_STACK_H
