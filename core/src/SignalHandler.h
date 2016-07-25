/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/SignalSet>

namespace cc {

class SignalHandler
{
protected:
    SignalHandler(SignalSet *listenSet = 0);
    int waitForSignal();

private:
    Ref<SignalSet> listenSet_;
};

} // namespace cc
