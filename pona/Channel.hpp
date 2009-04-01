/*
 * Channel.hpp -- fixed length message queue
 *
 * Copyright (c) 2007-2009, Frank Mertens
 *
 * See ../LICENSE for the license.
 */

#ifndef PONA_CHANNEL_HPP
#define PONA_CHANNEL_HPP

#include "atoms"
#include "Queue.hpp"
#include "Mutex.hpp"
#include "Condition.hpp"

namespace pona
{

template<class T>
class Channel: public Instance
{
public:
	enum Where { Back, Front };
	
	Channel(int size): queue_(size) {}
	
	inline void pushBack(T e) { push(e, Back); }
	inline void pushFront(T e) { push(e, Front); }
	inline T popBack() { return pop(Back); }
	inline T popFront() { return pop(Front); }
	
	void push(T e, int where = Back)
	{
		mutex_.acquire();
		while (queue_.fill() == queue_.size())
			notFull_.wait(&mutex_);
		if (where == Front)
			queue_.pushFront(e);
		else
			queue_.pushBack(e);
		notEmpty_.signal();
		mutex_.release();
	}
	
	T pop(int where = Front)
	{
		mutex_.acquire();
		while (queue_.fill() == 0)
			notEmpty_.wait(&mutex_);
		T e = (where == Front) ? queue_.popFront() : queue_.popBack();
		notFull_.signal();
		mutex_.release();
		return e;
	}
	
private:
	Queue<T> queue_;
	Mutex mutex_;
	Condition notEmpty_, notFull_;
};

} // namespace pona

#endif // PONA_CHANNEL_HPP
