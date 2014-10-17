/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <signal.h>
#include <flux/SignalMaster>

namespace flux {

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
        if (error != 0) FLUX_SYSTEM_DEBUG_ERROR(error);
        receivedSignals_->pushBack(signal);
        if (terminationSet_->contains(signal)) break;
    }
}

} // namespace flux
