/*
 * Copyright (C) 2007-2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/SignalMaster>
#include <cc/exceptions>
#include <signal.h>

namespace cc {

Ref<SignalMaster> SignalMaster::start(const SignalHandler &handler)
{
    Ref<SignalMaster> master = new SignalMaster{handler};
    master->Thread::start();
    return master;
}

SignalMaster::SignalMaster(const SignalHandler &handler):
    handler_{handler}
{}

void SignalMaster::run()
{
    auto all = SignalSet::full();

    for (bool fin = false; !fin;) {
        int signal = 0;
        int error = ::sigwait(all, &signal);
        if (error != 0) CC_SYSTEM_DEBUG_ERROR(error);
        handler_(static_cast<SystemSignal>(signal), &fin);
    }
}

} // namespace cc
