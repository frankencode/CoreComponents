/*
 * Channel.hpp -- generic M to N thread communication and synchronisation media
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../COPYING for the license.
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
	Channel(int size = 0)
		: queue_(size + (size == 0)),
		  ackMode_(size == 0)
	{}
	
	Channel& push(const T& item)
	{
		queueMutex_.acquire();
		while (queue_.fill() == queue_.size())
			notFull_.wait(&queueMutex_);
		queue_.push(item);
		if (ackMode_) {
			ackMutex_.acquire();
			ack_.signal();
			notEmpty_.signal();
			queueMutex_.release();
			ack_.wait(&ackMutex_);
			ackMutex_.release();
		}
		else {
			notEmpty_.signal();
			queueMutex_.release();
		}
		return *this;
	}
	
	Channel& pop(T& item)
	{
		queueMutex_.acquire();
		while (queue_.fill() == 0)
			notEmpty_.wait(&queueMutex_);
		if (ackMode_) {
			ackMutex_.acquire();
			ack_.signal();
			ackMutex_.release();
		}
		item = queue_.pop();
		notFull_.signal();
		queueMutex_.release();
		return *this;
	}
	
	inline T pop() {
		T item;
		pop(item);
		return item;
	}
	
private:
	inline bool isEmpty() const { return false; }
	
	Queue<T> queue_;
	Mutex queueMutex_;
	Condition notEmpty_, notFull_;
	bool ackMode_;
	Mutex ackMutex_;
	Condition ack_;
};

} // namespace ftl

#endif // FTL_CHANNEL_HPP
