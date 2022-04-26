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

namespace cc {

void Timer::startAt(double startTime)
{
    me().isActive_ = true;
    me().firstTime_ = me().startTime_ = startTime;
    TimeMaster{}.startTimer(*this);
}

void Timer::startIn(double delayTime)
{
    me().isActive_ = true;
    me().startTime_ = System::now();
    me().firstTime_ = me().startTime_ + delayTime;
    TimeMaster{}.startTimer(*this);
}

void Timer::stop()
{
    me().isActive_ = false;
}

} // namespace cc
