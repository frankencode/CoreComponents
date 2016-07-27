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

namespace cc {

/** \class Clock Clock.h cc/Clock
  * \brief %Clock generator thread
  *
  * A Clock is a thread that writes time beats into a channel in a fixed time interval.
  * The time interval may not be hit exactly for each tick, but the overall time scale
  * is maintained. In other words an individual tick may be delivered unprecisely due
  * to the uncertainty of thread scheduling, but there is no error accumulated in the
  * process.
  *
  * \see Channel, Thread, System::now()
  */
class Clock: public Thread
{
public:
    typedef Channel<double> BeatChannel;

    inline static Ref<Clock> start(double startTime, double interval = 1, BeatChannel *beatChannel = 0) {
        return new Clock(startTime, interval, beatChannel);
    }

    static double nextIntervalStart(double startTime, double interval);

    inline double interval() const { return interval_; }
    inline double startTime() const { return startTime_; }
    inline BeatChannel *beatChannel() const { return beatChannel_; }

private:
    Clock(double interval, double startTime, BeatChannel *beatChannel);
    ~Clock();

    virtual void run();

    typedef Channel<bool> ShutdownChannel;

    double interval_;
    double startTime_;
    Ref<BeatChannel> beatChannel_;
    Ref<ShutdownChannel> shutdown_;
};

} // namespace cc
