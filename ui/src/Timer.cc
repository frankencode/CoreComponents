/*
 * Copyright (C) 2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/System>
#include <cc/ui/TimeMaster>
#include <cc/ui/Timer>

namespace cc {
namespace ui {

void Timer::start()
{
    startIn(interval_);
}

void Timer::startAt(double startTime)
{
    if (isActive_) return;
    isActive_ = true;
    firstTime_ = startTime_ = startTime;
    TimeMaster::instance()->startTimer(this);
}

void Timer::startIn(double delayTime)
{
    if (isActive_) return;
    isActive_ = true;
    startTime_ = System::now();
    firstTime_ = startTime_ + delayTime;
    TimeMaster::instance()->startTimer(this);
}

void Timer::stop()
{
    isActive_ = false;
}

}} // namespace cc::ui
