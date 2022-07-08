/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/TimeMaster>
#include <cc/PlatformPlugin>

namespace cc {

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

} // namespace cc
