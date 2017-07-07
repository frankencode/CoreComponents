/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <signal.h>
#include <cc/exceptions>
#include <cc/SignalMaster>

namespace cc {

Ref<SignalMaster> SignalMaster::create(SignalSet *listenSet, SignalSet *terminationSet)
{
    return new SignalMaster(listenSet, terminationSet);
}

SignalMaster::SignalMaster(SignalSet *listenSet, SignalSet *terminationSet):
    listenSet_(listenSet),
    terminationSet_(terminationSet),
    receivedSignals_(SignalChannel::create())
{
    if (!listenSet_) listenSet_ = SignalSet::createFull();
    if (!terminationSet_) {
        terminationSet_ = SignalSet::createEmpty();
        terminationSet_->insert(SIGINT);
        terminationSet_->insert(SIGTERM);
    }
}

void SignalMaster::run()
{
    int signal = 0;
    while (true) {
        int error = ::sigwait(listenSet_->rawSet(), &signal);
        if (error != 0) CC_SYSTEM_DEBUG_ERROR(error);
        if (signal == SIGCHLD) continue;
        receivedSignals_->pushBack(signal);
        if (terminationSet_->contains(signal)) break;
    }
}

} // namespace cc
