/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/Thread>
#include <cc/Channel>
#include <cc/System>

namespace cc {

/** \brief interval timer thread
  *
  * A Timer is a thread that writes tick objects into a channel in a fixed time interval.
  * The time interval may not be hit exactly for each tick, but the overall time scale
  * is maintained. In other words an individual tick may be delivered unprecisely due
  * to the uncertainty of thread scheduling, but there is no error accumulated in the
  * process.
  *
  * \see Channel, Thread, System::now()
  */
template<class T>
class Timer: public Thread
{
public:
    static Ref<Timer> start(double startTime, double interval, Channel<T> *triggered = 0, T tick = T()) {
        return new Timer(startTime, interval, triggered, tick);
    }

    inline double startTime() const { return startTime_; }
    inline double interval() const { return interval_; }
    inline Channel<T> *triggered() const { return triggered_; }
    inline T tick() const { return tick_; }

protected:
    Timer(double startTime, double interval, Channel<T> *triggered, T tick):
        startTime_(startTime),
        interval_(interval),
        triggered_(triggered),
        shutdown_(Channel<bool>::create()),
        tick_(tick)
    {
        if (!triggered_) triggered_ = Channel<T>::create();
        Thread::start();
    }

    ~Timer()
    {
        shutdown_->push(true);
        Thread::wait();
    }

    virtual void writeTick(double timeout, bool shutdown)
    {
        if (!shutdown) triggered()->push(tick());
    }

private:
    virtual void run()
    {
        double timeout = startTime_;
        while (!shutdown_->popBefore(timeout)) {
            writeTick(timeout, false);
            timeout += interval_;
        }
        writeTick(timeout, true);
    }

    double startTime_;
    double interval_;
    Ref< Channel<T> > triggered_;
    Ref< Channel<bool> > shutdown_;
    T tick_;
};

} // namespace cc

