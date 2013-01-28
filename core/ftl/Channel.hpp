 /*
  * Copyright (C) 2007-2013 Frank Mertens.
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version
  * 2 of the License, or (at your option) any later version.
  */
#ifndef FTL_CHANNEL_HPP
#define FTL_CHANNEL_HPP

#include "Queue.hpp"
#include "Mutex.hpp"
#include "Condition.hpp"

namespace ftl
{

template<class T>
class Channel: public Container< T, Channel<T> >
{
public:
	static O<Channel> create() {
		return new Channel;
	}

	void pushBack(const T &item)
	{
		Guard<Mutex> guard(mutex_);
		queue_->pushBack(item);
		notEmpty_->signal();
	}

	void pushFront(const T &item)
	{
		Guard<Mutex> guard(mutex_);
		queue_->pushFront(item);
		notEmpty_->signal();
	}

	void popBack(T *item)
	{
		Guard<Mutex> guard(mutex_);
		while (queue_->length() == 0)
			notEmpty_->wait(mutex_);
		queue_->popBack(item);
	}

	void popFront(T *item)
	{
		Guard<Mutex> guard(mutex_);
		while (queue_->length() == 0)
			notEmpty_->wait(mutex_);
		queue_->popFront(item);
	}

	inline T popFront() {
		T item;
		popFront(&item);
		return item;
	}

	inline void push(const T &item) { return pushBack(item); }
	inline void pop(T *item) { return popFront(item); }
	inline T pop() { return popFront(); }

	inline bool isEmpty() const { return false; }

protected:
	Channel()
		: queue_(Queue<T>::create()),
		  mutex_(Mutex::create()),
		  notEmpty_(Condition::create())
	{}

private:
	O< Queue<T> > queue_;
	O<Mutex> mutex_;
	O<Condition> notEmpty_;
};

} // namespace ftl

#endif // FTL_CHANNEL_HPP
