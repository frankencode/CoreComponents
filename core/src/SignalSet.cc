/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/exceptions>
#include <cc/SignalSet>

namespace cc {

SignalSet::SignalSet(int preset)
{
    if (preset == Empty) sigemptyset(&rawSet_);
    else if (preset == Full) sigfillset(&rawSet_);
    else {
        int ret = pthread_sigmask(0, 0, &rawSet_);
        if (ret != 0) CC_SYSTEM_DEBUG_ERROR(ret);
    }
}

void SignalSet::insert(int signal) { sigaddset(&rawSet_, signal); }
void SignalSet::remove(int signal) { sigdelset(&rawSet_, signal); }
bool SignalSet::contains(int signal) const { return sigismember(&rawSet_, signal); }

} // namespace cc
