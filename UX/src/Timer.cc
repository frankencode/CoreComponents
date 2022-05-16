/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Timer>
#include <cc/TimeMaster>
#include <cc/System>
#include <cc/Application>

namespace cc {

Timer &Timer::startAt(double startTime)
{
    me().isActive_ = true;
    me().firstTime_ = me().startTime_ = startTime;
    TimeMaster{}.startTimer(*this);
    return *this;
}

Timer &Timer::startIn(double delayTime)
{
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

int Timer::run()
{
    return Application{}.run();
}

} // namespace cc
