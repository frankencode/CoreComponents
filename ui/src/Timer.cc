/*
 * Copyright (C) 2017-2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/Timer>
#include <cc/ui/TimeMaster>
#include <cc/System>

namespace cc {
namespace ui {

Timer::Instance::Instance(double interval):
    interval_{interval}
{}

void Timer::Instance::start()
{
    startIn(interval_);
}

void Timer::Instance::startAt(double startTime)
{
    isActive_ = true;
    firstTime_ = startTime_ = startTime;
    TimeMaster::instance()->startTimer(this);
}

void Timer::Instance::startIn(double delayTime)
{
    isActive_ = true;
    startTime_ = System::now();
    firstTime_ = startTime_ + delayTime;
    TimeMaster::instance()->startTimer(this);
}

void Timer::Instance::stop()
{
    isActive_ = false;
}

}} // namespace cc::ui
