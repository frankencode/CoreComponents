/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUX_THREAD_H
#define FLUX_THREAD_H

#include <pthread.h>
#include <signal.h>
#include <flux/SignalSet>
#include <flux/ThreadLocalRef>
#include <flux/String>

namespace flux {

class ThreadFactory;

/** \brief Thread creation, signalling, termination
  * \see Process
  */
class Thread: public Object
{
public:
    static Thread *self();

    void start(int stackSize = -1);
    void wait();
    void kill(int signal);
    bool stillAlive() const;

    static void sleep(double duration);
    static void sleepUntil(double timeout);

    static void blockSignals(SignalSet *set);
    static void unblockSignals(SignalSet *set);

    pthread_t id() const { return tid_; }

protected:
    Thread(): lastSignal_(0) {}

    virtual void run();
    virtual void handleSignal(int signal);

private:
    friend class ThreadFactory;
    friend class Process;

    static ThreadLocalRef<Thread> self_;
    Ref<ByteArray> stack_;
    pthread_t tid_;
    int lastSignal_;
};

inline Thread *thread() { return Thread::self(); }

} // namespace flux

#endif // FLUX_THREAD_H
