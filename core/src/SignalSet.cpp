/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <flux/SignalSet>

namespace flux {

SignalSet::SignalSet(int preset)
{
    if (preset == Empty) sigemptyset(&rawSet_);
    else if (preset == Full) sigfillset(&rawSet_);
    else {
        int ret = pthread_sigmask(0, 0, &rawSet_);
        if (ret != 0) FLUX_SYSTEM_DEBUG_ERROR(ret);
    }
}

void SignalSet::insert(int signal) { sigaddset(&rawSet_, signal); }
void SignalSet::remove(int signal) { sigdelset(&rawSet_, signal); }
bool SignalSet::contains(int signal) const { return sigismember(&rawSet_, signal); }

} // namespace flux
