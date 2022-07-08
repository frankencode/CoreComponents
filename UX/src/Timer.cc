/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/TimerState>
#include <cc/TimeMaster>
#include <cc/System>
#include <cc/Application>

namespace cc {

Timer::Timer(double interval):
    Object{new State{interval}}
{}

Timer &Timer::onTimeout(Fun<void()> &&f)
{
    me().timeout_(move(f));
    return *this;
}

Timer &Timer::start()
{
    startIn(me().interval_);
    return *this;
}

Timer &Timer::startAt(double startTime)
{
    if (me().startTime_ > 0) {
        me().isActive_ = false;
        Timer newTimer{me().interval_};
        newTimer->timeout_ = me().timeout_;
        *this = newTimer;
    }
    me().isActive_ = true;
    me().firstTime_ = me().startTime_ = startTime;
    TimeMaster{}.startTimer(*this);
    return *this;
}

Timer &Timer::startIn(double delayTime)
{
    if (me().startTime_ > 0) {
        me().isActive_ = false;
        Timer newTimer{me().interval_};
        newTimer->timeout_ = me().timeout_;
        *this = newTimer;
    }
    me().isActive_ = true;
    me().startTime_ = System::now();
    me().firstTime_ = me().startTime_ + delayTime;
    TimeMaster{}.startTimer(*this);
    return *this;
}

void Timer::stop()
{
    me().isActive_ = false;
}

bool Timer::isActive() const
{
    return me().isActive_;
}

double Timer::firstTime() const
{
    return me().firstTime_;
}

double Timer::startTime() const
{
    return me().startTime_;
}

double Timer::interval() const
{
    return me().interval_;
}

int Timer::run()
{
    return Application{}.run();
}

Timer::State &Timer::me()
{
    return Object::me.as<State>();
}

const Timer::State &Timer::me() const
{
    return Object::me.as<State>();
}

Timer::State *Timer::operator->()
{
    return &me();
}

const Timer::State *Timer::operator->() const
{
    return &me();
}

} // namespace cc
