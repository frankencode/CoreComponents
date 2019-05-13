/*
 * Copyright (C) 2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/PlatformPlugin>
#include <cc/ui/TimeWorker>
#include <cc/ui/TimeMaster>

namespace cc {
namespace ui {

TimeMaster *TimeMaster::instance()
{
    return PlatformPlugin::instance()->timeMaster();
}

TimeMaster::TimeMaster():
    worker_{TimeWorker::create(this)}
{
    worker_->start();
}

TimeMaster::~TimeMaster()
{
    worker_->shutdown();
    worker_->wait();
}

void TimeMaster::startTimer(Timer *timer)
{
    worker_->startTimer(timer);
}

void TimeMaster::ack()
{
    worker_->ack();
}

}} // namespace cc::ui
