/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FKIT_TIMER_H
#define FKIT_TIMER_H

#include "Thread.h"
#include "Channel.h"
#include "System.h"

namespace fkit
{

template<class T>
class Timer: public Thread
{
public:
	static Ref<Timer> start(double interval, Channel<T> *triggered, T tick = T()) {
		return new Timer(interval, triggered, tick);
	}

	inline T tick() const { return tick_; }

private:
	Timer(double interval, Channel<T> *triggered, T tick)
		: interval_(interval),
		  triggered_(triggered),
		  tick_(tick),
		  shutdown_(Channel<bool>::create())
	{
		Thread::start(/*4096*/);
	}

	~Timer() {
		shutdown_->push(true);
		Thread::wait();
	}

	virtual void run() {
		double timeout = System::now() + interval_;
		while (!shutdown_->popBefore(timeout)) {
			timeout += interval_;
			triggered_->push(tick_);
		}
	}

	double interval_;
	Ref< Channel<T> > triggered_;
	T tick_;
	Ref< Channel<bool> > shutdown_;
};

} // namespace fkit

#endif // FKIT_TIMER_H
