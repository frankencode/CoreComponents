/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUX_SIGNALMASTER_H
#define FLUX_SIGNALMASTER_H

#include <flux/Thread>
#include <flux/Channel>
#include <flux/SignalSet>

namespace flux {

typedef Channel<int> SignalChannel;

/** \brief Signal handling thread in a mulithreaded application
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

} // namespace flux

#endif // FLUX_SIGNALMASTER_H
