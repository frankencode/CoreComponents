/*
 * Copyright (C) 2024 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/Sdl3TimeMaster>
#include <cc/Sdl3Application>

namespace cc {

struct Sdl3TimeMaster::State final: public TimeMaster::State
{
    void triggerTimer(const Timer &timer) override
    {
        Sdl3Application{}.triggerTimer(timer);
    }
};

Sdl3TimeMaster::Sdl3TimeMaster():
    TimeMaster{instance<State>()}
{}

void Sdl3TimeMaster::shutdown()
{
    Singleton::destroy<State>();
}

} // namespace cc
