/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/TimeWorker>
#include <cc/TimeMaster>
#include <cc/Timer>
#include <cc/TimerState>
#include <cc/System>
#include <cc/Semaphore>
#include <cc/Channel>
#include <cc/Thread>
#include <cc/MultiSet>
#include <cmath>

namespace cc {

class TimeoutRequest: public Object
{
public:
    TimeoutRequest() = default;

    TimeoutRequest(long id, Timer timer):
        Object{new State{id, timer}}
    {
        timer->requestId_ = id;
    }

    bool isActive() const
    {
        return me().timer_.isActive() && me().timer_->requestId_ == me().id_;
    }

    Timer timer() const { return me().timer_; }

    double nextTime() const { return me().nextTime_; }

    double &nextTime() { return me().nextTime_; }

    double interval() const { return me().interval_; }

    void deactivate()
    {
        me().timer_->isActive_ = false;
    }

    bool operator<(const TimeoutRequest &other) const
    {
        return nextTime() < other.nextTime();
    }

private:
    struct State: public Object::State
    {
        State(long id, const Timer &timer):
            id_{id},
            timer_{timer},
            nextTime_{timer.firstTime()},
            interval_{timer.interval()}
        {}

        long id_;
        Timer timer_;
        double nextTime_;
        double interval_;
    };

    State &me() { return Object::me.as<State>(); }
    const State &me() const { return Object::me.as<State>(); }
};

struct TimeWorker::State: public Object::State
{
    State():
        thread_{[this]{ run(); }}
    {}

    void start()
    {
        thread_.start();
    }

    void run()
    {
        while (true)
        {
            TimeoutRequest request;

            if (timeoutRequests_.count() > 0) {
                TimeoutRequest timeout = timeoutRequests_.min();

                if (!startRequests_.popFrontBefore(timeout.nextTime(), &request))
                {
                    if (!timeout.isActive()) {
                        timeoutRequests_.removeAt(0);
                        continue;
                    }

                    TimeMaster{}.triggerTimer(timeout.timer());

                    if (timeout.interval() == 0) {
                        timeout.deactivate();
                        timeoutRequests_.removeAt(0);
                        continue;
                    }

                    {
                        double t = System::now();
                        double &tn = timeout.nextTime();
                        double T = timeout.interval();
                        if (T < t - tn) tn += std::floor((t - tn) / T) * T;
                        tn += T;
                    }
                    timeoutRequests_.removeAt(0);
                    ack_.acquire();
                    timeoutRequests_.insert(timeout);
                    continue;
                }
            }
            else {
                startRequests_.popFront(&request);
            }

            if (!request) break;
            timeoutRequests_.insert(request);
        }
    }

    long nextRequestId_ { 0 };
    Channel<TimeoutRequest> startRequests_;
    MultiSet<TimeoutRequest> timeoutRequests_;
    Semaphore<int> ack_;
    Thread thread_;
};

TimeWorker::TimeWorker():
    Object{new State}
{
    me().start();
}

TimeWorker::~TimeWorker()
{}

void TimeWorker::startTimer(const Timer &timer)
{
    me().startRequests_.emplaceBack(
        ++me().nextRequestId_, timer
    );
}

void TimeWorker::ack()
{
    me().ack_.release();
}

void TimeWorker::shutdown()
{
    me().startRequests_.emplaceBack();
    me().ack_.release();
    me().thread_.wait();
}

TimeWorker::State &TimeWorker::me()
{
    return Object::me.as<State>();
}

const TimeWorker::State &TimeWorker::me() const
{
    return Object::me.as<State>();
}

} // namespace cc
