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

Clock::Clock(double interval, double startTime, BeatChannel *beatChannel):
    interval_(interval),
    startTime_(startTime),
    beatChannel_(beatChannel),
    shutdown_(ShutdownChannel::create())
{
    if (!beatChannel_) beatChannel_ = BeatChannel::create();
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
        beatChannel_->push(timeout);
        timeout += interval_;
    }
}

} // namespace flux
