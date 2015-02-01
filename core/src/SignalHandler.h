/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUX_SIGNALHANDLER_H
#define FLUX_SIGNALHANDLER_H

#include <flux/SignalSet>

namespace flux {

class SignalHandler
{
protected:
    SignalHandler(SignalSet *listenSet = 0);
    int waitForSignal() const;

private:
    Ref<SignalSet> listenSet_;
};

} // namespace flux

#endif // FLUX_SIGNALHANDLER_H
