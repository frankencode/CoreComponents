/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUX_CLOCK_H
#define FLUX_CLOCK_H

#include <flux/Thread>
#include <flux/Channel>

namespace flux {

/** \brief Clock generator thread
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

    inline static Ref<Clock> start(double startTime, double interval = 1, BeatChannel *beat = 0) {
        return new Clock(startTime, interval, beat);
    }

    static double nextIntervalStart(double startTime, double interval);

    inline double interval() const { return interval_; }
    inline double startTime() const { return startTime_; }
    inline BeatChannel *beat() const { return beat_; }

private:
    Clock(double interval, double startTime, BeatChannel *beat);
    ~Clock();

    virtual void run();

    typedef Channel<bool> ShutdownChannel;

    double interval_;
    double startTime_;
    Ref<BeatChannel> beat_;
    Ref<ShutdownChannel> shutdown_;
};

} // namespace flux

#endif // FLUX_CLOCK_H
