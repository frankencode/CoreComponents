/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/SignalHandler>
#include <cc/exceptions>

namespace cc
{

SignalHandler::SignalHandler(SignalSet *listenSet):
    listenSet_(listenSet)
{
    if (!listenSet_) listenSet_ = SignalSet::createFull();
}

int SignalHandler::waitForSignal()
{
    int signal = 0;
    int error = ::sigwait(listenSet_->rawSet(), &signal);
    if (error != 0) CC_SYSTEM_DEBUG_ERROR(error);
    return signal;
}

} // namespace cc
