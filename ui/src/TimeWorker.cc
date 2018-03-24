/*
 * Copyright (C) 2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <math.h>
#include <cc/System>
#include <cc/ui/TimeMaster>
#include <cc/ui/TimeWorker>

namespace cc {
namespace ui {

Ref<TimeWorker> TimeWorker::create(TimeMaster *master)
{
    return new TimeWorker(master);
}

TimeWorker::TimeWorker(TimeMaster *master):
    master_(master),
    startRequests_(StartRequests::create()),
    timeouts_(Timeouts::create()),
    ack_(Semaphore::create())
{}

void TimeWorker::startTimer(const Timer *timer)
{
    startRequests_->pushBack(Timeout::create(timer));
}

void TimeWorker::ack()
{
    ack_->release();
}

void TimeWorker::shutdown()
{
    startRequests_->pushBack(Ref<Timeout>());
    ack_->release();
}

void TimeWorker::run()
{
    for (uint64_t nextSerial = 0; true;)
    {
        Ref<Timeout> request;

        if (timeouts_->count() > 0) {
            Ref<Timeout> timeout = timeouts_->valueAt(0);

            if (!startRequests_->popBackBefore(timeout->nextTime_, &request))
            {
                if (!timeout->timer_->isActive_) {
                    timeouts_->removeAt(0);
                    continue;
                }

                master_->triggerTimer(timeout->timer_);
                if (timeout->interval_ == 0) {
                    timeout->timer_->isActive_ = false;
                    timeouts_->removeAt(0);
                    continue;
                }

                {
                    double t = System::now();
                    double &tn = timeout->nextTime_;
                    double T = timeout->interval_;
                    if (T < t - tn) tn += ::floor((t - tn) / T) * T;
                    tn += T;
                }
                timeouts_->removeAt(0);
                ack_->acquire();
                timeouts_->insert(TimeKey(timeout->nextTime_, ++nextSerial), timeout);
                continue;
            }
        }
        else
            startRequests_->popBack(&request);

        if (!request) break;
        timeouts_->insert(TimeKey(request->nextTime_, ++nextSerial), request);
    }
}

}} // namespace cc::ui
