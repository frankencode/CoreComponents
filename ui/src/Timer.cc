/*
 * Copyright (C) 2017-2019 Frank Mertens.
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

void Timer::start(double interval, const std::function<void()> &handler)
{
    Ref<Timer> timer = Timer::create(interval);
    timer->triggered->connect(handler);
    timer->start();
}

void Timer::startAt(double startTime, double interval, const std::function<void()> &handler)
{
    Ref<Timer> timer = Timer::create(interval);
    timer->triggered->connect(handler);
    timer->startAt(startTime);
}

void Timer::startIn(double delayTime, const std::function<void()> &handler)
{
    Ref<Timer> timer = Timer::create();
    timer->triggered->connect(handler);
    timer->startIn(delayTime);
}

Ref<Timer> Timer::create(double interval)
{
    return new Timer{interval};
}

Timer::Timer(double interval):
    interval_{interval}
{}

void Timer::start()
{
    startIn(interval_);
}

void Timer::startAt(double startTime)
{
    isActive_ = true;
    firstTime_ = startTime_ = startTime;
    TimeMaster::instance()->startTimer(this);
}

void Timer::startIn(double delayTime)
{
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
