/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/TimeMaster>
#include <cc/ui/PlatformPlugin>

namespace cc::ui {

TimeMaster::TimeMaster()
{
    *this = platform().timeMaster();
}

void TimeMaster::startTimer(const Timer &timer)
{
    me().worker_.startTimer(timer);
}

void TimeMaster::ack()
{
    me().worker_.ack();
}

TimeMaster::State::~State()
{
    worker_.shutdown();
}

} // namespace cc::ui
