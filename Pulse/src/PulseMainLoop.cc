/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/PulseMainLoop>

namespace cc {

struct PulseMainLoop::State final: public Object::State
{
    State():
        mainLoop_{pa_mainloop_new()}
    {}

    ~State()
    {
        pa_mainloop_free(mainLoop_);
    }

    pa_mainloop *mainLoop_ { nullptr };
};

PulseMainLoop::PulseMainLoop():
    Object{new State}
{}

int PulseMainLoop::run()
{
    int status = 0;
    CC_PULSE(pa_mainloop_run(me().mainLoop_, &status));
    return status;
}

/** %Quit the %Pulse %Audio event loop
  */
void PulseMainLoop::quit(int status)
{
    pa_mainloop_quit(me().mainLoop_, status);
}

/** %Get access to the event loop's API
  */
pa_mainloop_api *PulseMainLoop::api()
{
    return pa_mainloop_get_api(me().mainLoop_);
}

PulseMainLoop::State &PulseMainLoop::me()
{
    return Object::me.as<State>();
}

const PulseMainLoop::State &PulseMainLoop::me() const
{
    return Object::me.as<State>();
}

} // namespace cc
