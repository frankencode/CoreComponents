/*
 * Copyright (C) 2007-2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <sys/mman.h>
#include <time.h>
#include <math.h>
#include <limits.h>
#include <cc/strings>
#include <cc/exceptions>
#include <cc/System>
#include <cc/Thread>

namespace cc {

thread_local Ref<Thread> Thread::self_;

Thread *Thread::self()
{
    if (!self_) {
        self_ = new Thread;
        self_->tid_ = pthread_self();
    }
    return self_;
}

void *Thread::bootstrap(void *self)
{
    Thread *thread = static_cast<Thread *>(self);
    Thread::self_ = thread;
    thread->run();
    Thread::self_ = 0;
    return (void *)thread;
}

Thread *Thread::start()
{
    pthread_attr_t attr;
    {
        int ret = pthread_attr_init(&attr);
        if (ret != 0) CC_SYSTEM_DEBUG_ERROR(ret);
    }

    if (stackSize_ > 0) {
        if (stackSize_ < PTHREAD_STACK_MIN) stackSize_ = PTHREAD_STACK_MIN;
        int ret = pthread_attr_setstacksize(&attr, stackSize_);
        if (ret != 0) CC_SYSTEM_DEBUG_ERROR(ret);
    }

    int ret = pthread_create(&tid_, &attr, &bootstrap, static_cast<void *>(this));
    if (ret != 0) CC_SYSTEM_DEBUG_ERROR(ret);

    return this;
}

void Thread::wait()
{
    int ret = pthread_join(tid_, 0);
    if (ret != 0) CC_SYSTEM_DEBUG_ERROR(ret);
}

void Thread::kill(int signal)
{
    int ret = pthread_kill(tid_, signal);
    if (ret != 0) CC_SYSTEM_DEBUG_ERROR(ret);
}

bool Thread::stillAlive() const
{
    int ret = pthread_kill(tid_, 0);
    if ((ret != 0) && (ret != ESRCH))
        CC_SYSTEM_DEBUG_ERROR(ret);
    return (ret == 0);
}

void Thread::sleep(double duration)
{
    struct timespec rqtp;
    double sec = 0;
    rqtp.tv_nsec = modf(duration, &sec) * 1e9;
    rqtp.tv_sec = sec;
    while (::nanosleep(&rqtp, &rqtp) == -1) {
        if (errno != EINTR)
            CC_SYSTEM_DEBUG_ERROR(errno);
    }
}

void Thread::sleepUntil(double timeout)
{
    double now = System::now();
    if (timeout <= now) return;
    sleep(now - timeout);
}

void Thread::blockSignals(SignalSet *set)
{
    Singleton<SignalSet>::instance();
    int ret = pthread_sigmask(SIG_BLOCK, set->rawSet(), 0/*oldset*/);
    if (ret != 0) CC_SYSTEM_DEBUG_ERROR(ret);
}

void Thread::unblockSignals(SignalSet *set)
{
    Singleton<SignalSet>::instance();
    int ret = pthread_sigmask(SIG_UNBLOCK, set->rawSet(), 0/*oldset*/);
    if (ret != 0) CC_SYSTEM_DEBUG_ERROR(ret);
}

void Thread::run()
{}

} // namespace cc
