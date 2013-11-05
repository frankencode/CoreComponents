/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUX_PRIORITYCHANNEL_H
#define FLUX_PRIORITYCHANNEL_H

#include "PriorityQueue.h"
#include "Mutex.h"
#include "Guard.h"
#include "Condition.h"

namespace flux
{

template<class T>
class PriorityChannel: public Object
{
public:
	static Ref<PriorityChannel> create() {
		return new PriorityChannel;
	}

	void pushBack(const T &item, int priority = 0)
	{
		Guard<Mutex> guard(mutex_);
		queue_->pushBack(item, priority);
		notEmpty_->signal();
	}

	void pushFront(const T &item, int priority = 0)
	{
		Guard<Mutex> guard(mutex_);
		queue_->pushFront(item, priority);
		notEmpty_->signal();
	}

	T popBack(T *item = 0)
	{
		Guard<Mutex> guard(mutex_);
		while (queue_->size() == 0)
			notEmpty_->wait(mutex_);
		return queue_->popBack(item);
	}

	T popFront(T *item = 0)
	{
		Guard<Mutex> guard(mutex_);
		while (queue_->size() == 0)
			notEmpty_->wait(mutex_);
		return queue_->popFront(item);
	}

	bool popBackBefore(double timeout, T *item)
	{
		Guard<Mutex> guard(mutex_);
		while (queue_->size() == 0) {
			if (!notEmpty_->waitUntil(timeout, mutex_))
				return false;
		}
		queue_->popBack(item);
		return true;
	}

	bool popFrontBefore(double timeout, T *item)
	{
		Guard<Mutex> guard(mutex_);
		while (queue_->size() == 0) {
			if (!notEmpty_->waitUntil(timeout, mutex_))
				return false;
		}
		queue_->popFront(item);
		return true;
	}

	inline void push(const T &item, int priority = 0) { return pushBack(item, priority); }
	inline T pop(T *item = 0) { return popFront(item); }

	inline bool popBefore(double timeout, T *item = 0) { return popFrontBefore(timeout, item); }

	inline int size() const {
		Guard<Mutex> guard(mutex_);
		return queue_->size();
	}

protected:
	PriorityChannel()
		: queue_(PriorityQueue<T>::create()),
		  mutex_(Mutex::create()),
		  notEmpty_(Condition::create())
	{}

private:
	Ref< PriorityQueue<T> > queue_;
	Ref<Mutex> mutex_;
	Ref<Condition> notEmpty_;
};

} // namespace flux

#endif // FLUX_PRIORITYCHANNEL_H
