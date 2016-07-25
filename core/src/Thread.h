/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <pthread.h>
#include <signal.h>
#include <cc/SignalSet>
#include <cc/String>

namespace cc {

/** \brief %Thread creation, signalling, termination
  * \see SubProcess
  */
class Thread: public Object
{
public:
    static Thread *self();

    void start();
    void wait();
    void kill(int signal);
    bool stillAlive() const;

    static void sleep(double duration);
    static void sleepUntil(double timeout);

    static void blockSignals(SignalSet *set);
    static void unblockSignals(SignalSet *set);

    pthread_t id() const { return tid_; }

protected:
    Thread():
        stackSize_(1 << 16),
        lastSignal_(0)
    {}

    void setStackSize(int stackSize) { stackSize_ = stackSize; }

    virtual void run();

private:
    static Ref<ByteArray> allocateStack(int stackSize, int guardSize);

    static void *bootstrap(void *self);

    static thread_local Ref<Thread> self_;
    int stackSize_;
    Ref<ByteArray> stack_;
    pthread_t tid_;
    int lastSignal_;
};

} // namespace cc
