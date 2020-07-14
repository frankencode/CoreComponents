/*
 * Copyright (C) 2007-2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Thread>
#include <cc/System>
#include <cc/strings>
#include <cc/exceptions>
#include <cc/strings>
#include <sys/mman.h>
#include <time.h>
#include <math.h>
#include <limits.h>

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
    if (
        ret == EDEADLK &&
        pthread_equal(pthread_self(), tid_)
    ) {
        ret = pthread_detach(tid_);
    }
    #ifndef NDEBUG
    if (ret != 0) CC_SYSTEM_DEBUG_ERROR(ret);
    #endif
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

void Thread::blockSignals(const SignalSet &set)
{
    int ret = pthread_sigmask(SIG_BLOCK, set, nullptr/*oldset*/);
    if (ret != 0) CC_SYSTEM_DEBUG_ERROR(ret);
}

void Thread::unblockSignals(const SignalSet &set)
{
    int ret = pthread_sigmask(SIG_UNBLOCK, set, nullptr/*oldset*/);
    if (ret != 0) CC_SYSTEM_DEBUG_ERROR(ret);
}

int Thread::getMinPriority(SchedulingPolicy policy)
{
    int ret = sched_get_priority_min(static_cast<int>(policy));
    if (ret == -1) CC_SYSTEM_DEBUG_ERROR(errno);
    return ret;
}

int Thread::getMaxPriority(SchedulingPolicy policy)
{
    int ret = sched_get_priority_max(static_cast<int>(policy));
    if (ret == -1) CC_SYSTEM_DEBUG_ERROR(errno);
    return ret;
}

void Thread::setSchedulingPolicy(SchedulingPolicy policy, int priority)
{
    struct sched_param param;
    memclr(&param, sizeof(param));
    param.sched_priority = priority;
    int ret = sched_setscheduler(0, static_cast<int>(policy), &param);
    if (ret != 0) CC_SYSTEM_DEBUG_ERROR(ret);
}

void Thread::run()
{}

} // namespace cc
