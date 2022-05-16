/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/pulse/MainLoop>

namespace cc::pulse {

struct MainLoop::State: public Object::State
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

MainLoop::MainLoop():
    Object{new State}
{}

int MainLoop::run()
{
    int status = 0;
    CC_PULSE(pa_mainloop_run(me().mainLoop_, &status));
    return status;
}

/** %Quit the %Pulse %Audio event loop
  */
void MainLoop::quit(int status)
{
    pa_mainloop_quit(me().mainLoop_, status);
}

/** %Get access to the event loop's API
  */
pa_mainloop_api *MainLoop::api()
{
    return pa_mainloop_get_api(me().mainLoop_);
}

MainLoop::State &MainLoop::me()
{
    return Object::me.as<State>();
}

const MainLoop::State &MainLoop::me() const
{
    return Object::me.as<State>();
}

} // namespace cc::pulse
