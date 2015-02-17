#include <math.h>
#include <flux/System>
#include <flux/Clock>

namespace flux {

/** Compute the start of the next interval.
  * For "instance nextInternvalStart(System::now(), 1)" delivers the start of the next full second.
  */
double Clock::nextIntervalStart(double startTime, double interval)
{
    return startTime + interval - fmod(startTime, interval);
}

Clock::Clock(double interval, double startTime, BeatChannel *beat):
    interval_(interval),
    startTime_(startTime),
    beat_(beat),
    shutdown_(ShutdownChannel::create())
{
    if (!beat_) beat_ = BeatChannel::create();
}

Clock::~Clock()
{
    shutdown_->push(true);
    Thread::wait();
}

void Clock::run()
{
    double timeout = startTime_;
    while (!shutdown_->popBefore(timeout)) {
        beat_->push(timeout);
        timeout += interval_;
    }
}

} // namespace flux
