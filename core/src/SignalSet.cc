/*
 * Copyright (C) 2007-2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/exceptions>
#include <cc/SignalSet>

namespace cc {

Ref<SignalSet> SignalSet::createEmpty()
{
    return new SignalSet{Preset::Empty};
}

Ref<SignalSet> SignalSet::createFull()
{
    return new SignalSet{Preset::Full};
}

Ref<SignalSet> SignalSet::create()
{
    return new SignalSet;
}

SignalSet::SignalSet(Preset preset)
{
    if (preset == Preset::Empty) sigemptyset(&rawSet_);
    else if (preset == Preset::Full) sigfillset(&rawSet_);
    else {
        int ret = pthread_sigmask(0, 0, &rawSet_);
        if (ret != 0) CC_SYSTEM_DEBUG_ERROR(ret);
    }
}

void SignalSet::insert(Signal signal)
{
    sigaddset(&rawSet_, +signal);
}

void SignalSet::remove(Signal signal)
{
    sigdelset(&rawSet_, +signal);
}

bool SignalSet::contains(Signal signal) const
{
    return sigismember(&rawSet_, +signal);
}

} // namespace cc
