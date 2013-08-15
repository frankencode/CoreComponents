/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FKIT_CHANNEL_H
#define FKIT_CHANNEL_H

#include "Queue.h"
#include "Mutex.h"
#include "Guard.h"
#include "Condition.h"

namespace fkit
{

template<class T>
class Channel: public Object
{
public:
	static Ref<Channel> create() {
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

	T popBack(T *item = 0)
	{
		Guard<Mutex> guard(mutex_);
		while (queue_->size() == 0)
			notEmpty_->wait(mutex_);
		T h;
		if (!item) item = &h;
		queue_->popBack(item);
		return *item;
	}

	T popFront(T *item = 0)
	{
		Guard<Mutex> guard(mutex_);
		while (queue_->size() == 0)
			notEmpty_->wait(mutex_);
		T h;
		if (!item) item = &h;
		queue_->popFront(item);
		return *item;
	}

	inline void push(const T &item) { return pushBack(item); }
	inline T pop(T *item = 0) { return popFront(item); }

	inline int size() const {
		Guard<Mutex> guard(mutex_);
		return queue_->size();
	}

protected:
	Channel()
		: queue_(Queue<T>::create()),
		  mutex_(Mutex::create()),
		  notEmpty_(Condition::create())
	{}

private:
	Ref< Queue<T> > queue_;
	Ref<Mutex> mutex_;
	Ref<Condition> notEmpty_;
};

} // namespace fkit

#endif // FKIT_CHANNEL_H
