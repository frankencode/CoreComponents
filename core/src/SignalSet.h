/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUX_SIGNALSET_H
#define FLUX_SIGNALSET_H

#include <signal.h>
#include <flux/Object>

namespace flux {

class Thread;

class SignalSet: public Object
{
public:
    inline static Ref<SignalSet> createEmpty() {
        return new SignalSet(Empty);
    }
    inline static Ref<SignalSet> createFull() {
        return new SignalSet(Full);
    }

    void insert(int signal);
    void remove(int signal);

    bool contains(int signal) const;

    inline sigset_t *rawSet() { return &rawSet_; }

private:
    friend class Thread;

    enum { Empty, Full };
    SignalSet(int preset);
    sigset_t rawSet_;
};

} // namespace flux

#endif // FLUX_SIGNALSET_H
