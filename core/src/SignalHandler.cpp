/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <flux/SignalHandler>
#include <flux/exceptions>

namespace flux
{

SignalHandler::SignalHandler(SignalSet *listenSet):
    listenSet_(listenSet)
{
    if (!listenSet_) listenSet_ = SignalSet::createFull();
}

int SignalHandler::waitForSignal() const
{
    int signal = 0;
    int error = ::sigwait(listenSet_->rawSet(), &signal);
    if (error != 0) FLUX_SYSTEM_DEBUG_ERROR(error);
    return signal;
}

} // namespace flux
