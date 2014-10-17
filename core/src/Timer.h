/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUX_TIMER_H
#define FLUX_TIMER_H

#include <flux/Thread>
#include <flux/Channel>
#include <flux/System>

namespace flux {

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
		Thread::start();
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

} // namespace flux

#endif // FLUX_TIMER_H
