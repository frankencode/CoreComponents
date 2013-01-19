 /*
  * Copyright (C) 2007-2013 Frank Mertens.
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version
  * 2 of the License, or (at your option) any later version.
  */
#ifndef FTL_CONVEYOR_HPP
#define FTL_CONVEYOR_HPP

#include "CircularBuffer.hpp"
#include "Mutex.hpp"
#include "Condition.hpp"

namespace ftl
{

template<class T>
class Conveyor: public Container< T, Conveyor<T> >
{
public:
	inline static Ref<Conveyor, Owner> create(int size) {
		return new Conveyor(size);
	}

	Conveyor &push(const T &item)
	{
		mutex_.acquire();
		while (buffer_.fill() == buffer_.size())
			notFull_.wait(&mutex_);
		buffer_.push(item);
		notEmpty_.signal();
		mutex_.release();
		return *this;
	}

	Conveyor &pop(T *item)
	{
		mutex_.acquire();
		while (buffer_.fill() == 0)
			notEmpty_.wait(&mutex_);
		buffer_.pop(item);
		notFull_.signal();
		mutex_.release();
		return *this;
	}

	inline T pop() {
		T item;
		pop(&item);
		return item;
	}

	inline bool isEmpty() const { return false; }

protected:
	Conveyor(int size)
		: buffer_(size)
	{}

private:
	CircularBuffer<T> buffer_;
	Mutex mutex_;
	Condition notEmpty_, notFull_;
};

} // namespace ftl

#endif // FTL_CONVEYOR_HPP
