/*
 * Copyright (C) 2007-2017 Frank Mertens.
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

/** \class Thread Thread.h cc/Thread
  * \ingroup concurrency
  * \brief %Thread creation, signalling, termination
  * \see Channel, SubProcess
  */
class Thread: public Object
{
public:
    /// The current thread object
    static Thread *self();

    /// Start thread execution
    Thread *start();

    /// Wait for termination
    void wait();

    /// Send a signal
    void kill(int signal);

    /// Check if the thread is still alive
    bool stillAlive() const;

    /** Suspend execution of the current thread
      * \param duration duration of the suspension in seconds
      */
    static void sleep(double duration);

    /** Suspend execution of the current thread
      * \param timeout absolute time when to resume execution in seconds
      * \see System::now()
      */
    static void sleepUntil(double timeout);

    /** Block a set of signals
      * \param set set of signals to block
      */
    static void blockSignals(SignalSet *set);

    /** Unblock a set of signals
      * \param set set of signals to unblock
      */
    static void unblockSignals(SignalSet *set);

    /// POSIX thread ID
    pthread_t id() const { return tid_; }

protected:
    /// Low-level initializer
    Thread():
        stackSize_(1 << 16),
        lastSignal_(0)
    {}

    /** %Set the stack size before starting thread execution
      * \param stackSize size of the program stack to allocate in bytes
      */
    void setStackSize(int stackSize) { stackSize_ = stackSize; }

    /// First method which gets invoked once thread execution is started.
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
