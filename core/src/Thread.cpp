/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <flux/strings>
#include <flux/Condition>
#include <flux/ThreadFactory>
#include <flux/System>
#include <flux/Thread>

namespace flux {

ThreadLocalRef<Thread> Thread::self_;

Thread *Thread::self()
{
    if (!self_) {
        self_ = new Thread;
        self_->tid_ = pthread_self();
    }
    return self_;
}


void Thread::start(int stackSize)
{
    Ref<ThreadFactory> factory = ThreadFactory::create();
    if (stackSize > 0)
        factory->setStackSize(stackSize);
    factory->start(this);
}

void Thread::wait()
{
    int ret = pthread_join(tid_, 0);
    if (ret != 0) FLUX_SYSTEM_DEBUG_ERROR(ret);
}

void Thread::kill(int signal)
{
    int ret = pthread_kill(tid_, signal);
    if (ret != 0) FLUX_SYSTEM_DEBUG_ERROR(ret);
}

bool Thread::stillAlive() const
{
    int ret = pthread_kill(tid_, 0);
    if ((ret != 0) && (ret != ESRCH))
        FLUX_SYSTEM_DEBUG_ERROR(ret);
    return (ret == 0);
}

void Thread::sleep(double duration)
{
    sleepUntil(System::now() + duration);
}

void Thread::sleepUntil(double timeout)
{
    Ref<Mutex> mutex = Mutex::create();
    Ref<Condition> condition = Condition::create();
    mutex->acquire();
    condition->waitUntil(timeout, mutex);
    mutex->release();
}

void Thread::blockSignals(SignalSet *set)
{
    Singleton<SignalSet>::instance();
    int ret = pthread_sigmask(SIG_BLOCK, set->rawSet(), 0/*oldset*/);
    if (ret != 0) FLUX_SYSTEM_DEBUG_ERROR(ret);
}

void Thread::unblockSignals(SignalSet *set)
{
    Singleton<SignalSet>::instance();
    int ret = pthread_sigmask(SIG_UNBLOCK, set->rawSet(), 0/*oldset*/);
    if (ret != 0) FLUX_SYSTEM_DEBUG_ERROR(ret);
}

void Thread::run()
{}

void Thread::handleSignal(int signal)
{}

} // namespace flux
