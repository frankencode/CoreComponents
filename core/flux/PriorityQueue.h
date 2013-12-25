/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUX_PRIORITYQUEUE_H
#define FLUX_PRIORITYQUEUE_H

#include "Map.h"
#include "Queue.h"

namespace flux
{

template<class T>
class PriorityQueue: public Object
{
public:
	inline static Ref<PriorityQueue> create() { return new PriorityQueue; }

	inline void pushBack(const T &item, int priority = 0)
	{
		inputQueue(priority)->pushBack(item);
	}

	inline void pushFront(const T &item, int priority = 0)
	{
		inputQueue(priority)->pushFront(item);
	}

	inline T popBack(T *item)
	{
		FLUX_ASSERT(size_ > 0);
		return outputQueue()->popBack(item);
	}

	inline T popFront(T *item)
	{
		FLUX_ASSERT(size_ > 0);
		return outputQueue()->popFront(item);
	}

	inline T popBack() {
		T item;
		return popBack(&item);
	}

	inline T popFront() {
		T item;
		return popFront(&item);
	}

	inline void push(const T &item, int priority) { pushBack(item, priority); }
	inline T pop(T *item) { return popFront(item); }
	inline T pop() { return popFront(); }

	inline int size() const { return size_; }

private:
	PriorityQueue()
		: queueByPriority_(QueueByPriority::create()),
		  size_(0)
	{}

	Queue<T> *inputQueue(int priority)
	{
		++size_;
		Queue<T> *queue = queueByPriority_->value(priority);
		if (!queue) {
			Ref< Queue<T> > newQueue = Queue<T>::create();
			queueByPriority_->insert(priority, newQueue);
			queue = newQueue;
		}
		return queue;
	}

	Ref< Queue<T> > outputQueue()
	{
		--size_;
		typename QueueByPriority::Item pair = queueByPriority_->at(queueByPriority_->size() - 1);
		if (pair->value()->size() == 1) queueByPriority_->remove(pair->key());
		return pair->value();
	}

	typedef Map<int, Ref< Queue<T> > > QueueByPriority;
	Ref<QueueByPriority> queueByPriority_;

	int size_;
};

} // namespace flux

#endif // FLUX_PRIORITYQUEUE_H
