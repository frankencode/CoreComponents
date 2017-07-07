/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

namespace cc {

/** \class SpinLock SpinLock.h cc/SpinLock
  * \brief %Thread synchronization primitive: spinning lock
  */
class SpinLock
{
public:
    SpinLock();
    ~SpinLock();
    bool tryAcquire();
    void acquire();
    void release();

private:
    void yield();
    volatile char flag_;
};

} // namespace cc
