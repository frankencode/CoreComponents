/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/SdlTimeMaster>
#include <cc/SdlApplication>
#include <SDL2/SDL.h>

namespace cc {

struct SdlTimeMaster::State final: public TimeMaster::State
{
    void triggerTimer(const Timer &timer) override
    {
        SdlApplication{}.triggerTimer(timer);
    }
};

SdlTimeMaster::SdlTimeMaster():
    TimeMaster{instance<State>()}
{}

void SdlTimeMaster::shutdown()
{
    Singleton::destroy<State>();
}

} // namespace cc
