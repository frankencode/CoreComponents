 /*
  * Copyright (C) 2007-2013 Frank Mertens.
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version
  * 2 of the License, or (at your option) any later version.
  */
#ifndef FTL_RONDEZVOUS_HPP
#define FTL_RONDEZVOUS_HPP

#include "containers.hpp"
#include "Mutex.hpp"
#include "Condition.hpp"

namespace ftl
{

template<class T>
class Rondezvous: public Container< T, Rondezvous<T> >
{
public:
	inline static Ref<Rondezvous, Owner> create() {
		return new Rondezvous;
	}

	Rondezvous &push(const T &item)
	{
		tableMutex_.acquire();
		while (isFull_)
			emptyCondition_.wait(&tableMutex_);
		table_ = item;
		isFull_ = true;
		ackMutex_.acquire();
		ack_.signal();
		fullCondition_.signal();
		tableMutex_.release();
		ack_.wait(&ackMutex_);
		ackMutex_.release();
		return *this;
	}

	Rondezvous &pop(T *item)
	{
		tableMutex_.acquire();
		while (!isFull_)
			fullCondition_.wait(&tableMutex_);
		ackMutex_.acquire();
		ack_.signal();
		ackMutex_.release();
		*item = table_;
		isFull_ = false;
		emptyCondition_.signal();
		tableMutex_.release();
		return *this;
	}

	inline T pop() {
		T item;
		pop(&item);
		return item;
	}

	inline bool isEmpty() const { return false; }

private:
	Rondezvous(): isFull_(false) {}

	T table_;
	Mutex tableMutex_;
	bool isFull_;
	Condition emptyCondition_;
	Condition fullCondition_;
	Mutex ackMutex_;
	Condition ack_;
};

} // namespace ftl

#endif // FTL_RONDEZVOUS_HPP
