/*
 * Channel.hpp -- message queue of unlimited length
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
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
	static Ref<Channel, Owner> newInstance() {
		return new Channel;
	}

	Channel& push(const T& item)
	{
		mutex_->acquire();
		queue_->push(item);
		notEmpty_->signal();
		mutex_->release();
		return *this;
	}

	Channel& pop(T* item)
	{
		mutex_->acquire();
		while (queue_->length() == 0)
			notEmpty_->wait(mutex_);
		queue_->pop(item);
		mutex_->release();
		return *this;
	}

	inline T pop() {
		T item;
		pop(&item);
		return item;
	}

	inline bool isEmpty() const { return false; }

protected:
	Channel()
		: queue_(Queue<T>::newInstance()),
		  mutex_(Mutex::newInstance()),
		  notEmpty_(Condition::newInstance())
	{}

private:
	Ref< Queue<T>, Owner > queue_;
	Ref<Mutex, Owner> mutex_;
	Ref<Condition, Owner> notEmpty_;
};

} // namespace ftl

#endif // FTL_CHANNEL_HPP
