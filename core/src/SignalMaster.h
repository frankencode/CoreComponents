/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/Thread>
#include <cc/Channel>
#include <cc/SignalSet>

namespace cc {

typedef Channel<int> SignalChannel;

/** \class SignalMaster SignalMaster.h cc/SignalMaster
  * \brief Signal handling thread in a mulithreaded application
  * \see SignalSet
  */
class SignalMaster: public Thread
{
public:
    static Ref<SignalMaster> create(SignalSet *listenSet = 0, SignalSet *terminationSet = 0);
    inline SignalChannel *receivedSignals() const { return receivedSignals_; }

private:
    SignalMaster(SignalSet *listenSet, SignalSet *terminationSet);
    virtual void run();

    Ref<SignalSet> listenSet_;
    Ref<SignalSet> terminationSet_;
    Ref<SignalChannel> receivedSignals_;
};

} // namespace cc
