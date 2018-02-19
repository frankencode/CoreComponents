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
    startAt(System::now() + interval_);
}

void Timer::startAt(double startTime)
{
    if (isActive_) return;
    isActive_ = true;
    startTime_ = startTime;
    TimeMaster::instance()->startTimer(this);
}

void Timer::startIn(double delayTime)
{
    startAt(System::now() + delayTime);
}

void Timer::stop()
{
    isActive_ = false;
}

}} // namespace cc::ui
