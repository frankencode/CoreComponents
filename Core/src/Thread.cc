/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/Thread>
#include <cc/System>
#include <cmath> // std::modf(3)
#include <utility>
#include <cassert>
#include <type_traits>
#include <limits.h> // PTHREAD_STACK_*
#include <time.h> // nanosleep(2)
#include <pthread.h>

namespace cc {

static_assert(std::is_fundamental<pthread_t>());

struct Thread::State final: public Object::State
{
    static void *bootstrap(void *self)
    {
        static_cast<State *>(self)->f_();
        return self;
    }

    State(Function<void()> &&f, long stackSize):
        f_{f}
    {
        {
            int ret = pthread_attr_init(&attr_);
            if (ret != 0) CC_SYSTEM_DEBUG_ERROR(ret);
        }

        if (stackSize > 0) {
            if (stackSize < PTHREAD_STACK_MIN) stackSize = PTHREAD_STACK_MIN;
            int ret = pthread_attr_setstacksize(&attr_, stackSize);
            if (ret != 0) CC_SYSTEM_DEBUG_ERROR(ret);
        }
    }

    void start()
    {
        int ret = pthread_create(&tid_, &attr_, &bootstrap, static_cast<void *>(this));
        if (ret != 0) CC_SYSTEM_DEBUG_ERROR(ret);
    }

    pthread_attr_t attr_;
    pthread_t tid_ = pthread_t{};
    Function<void()> f_;
};

Thread::Thread(Function<void()> &&f, long stackSize):
    Object{new State{move(f), stackSize}}
{}

void Thread::start()
{
    assert(!isNull());
    me().start();
}

bool Thread::started() const
{
    return !isNull() && me().tid_ != pthread_t{};
}

void Thread::wait()
{
    int ret = pthread_join(me().tid_, nullptr);
    if (ret != 0) CC_SYSTEM_DEBUG_ERROR(ret);
}

void Thread::kill(Signal signal)
{
    int ret = pthread_kill(me().tid_, +signal);
    if (ret != 0) CC_SYSTEM_DEBUG_ERROR(ret);
}

bool Thread::isRunning() const
{
    int ret = pthread_kill(me().tid_, 0);
    if (ret != 0 && ret != ESRCH) CC_SYSTEM_DEBUG_ERROR(ret);
    return (ret == 0);
}

void Thread::setName(const char *name)
{
    #ifdef _GNU_SOURCE
    int ret = pthread_setname_np(me().tid_, name);
    if (ret != 0) CC_SYSTEM_DEBUG_ERROR(ret);
    #endif
}

uint64_t Thread::id()
{
    return static_cast<uint64_t>(pthread_self());
}

void Thread::sleep(double duration)
{
    struct timespec rqtp;
    double sec = 0;
    rqtp.tv_nsec = std::modf(duration, &sec) * 1e9;
    rqtp.tv_sec = sec;
    while (::nanosleep(&rqtp, &rqtp) == -1) {
        if (errno != EINTR)
            CC_SYSTEM_DEBUG_ERROR(errno);
    }
}

void Thread::sleepUntil(double time)
{
    double now = System::now();
    if (time > now) sleep(time - now);
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

SignalSet Thread::signalMask()
{
    SignalSet set;
    pthread_sigmask(/*how*/0, /*new_set*/nullptr, /*old_set*/set);
    return set;
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
    std::memset(&param, 0, sizeof(param));
    param.sched_priority = priority;
    int ret = sched_setscheduler(0, +policy, &param);
    if (ret != 0) CC_SYSTEM_DEBUG_ERROR(ret);
}

Thread::State &Thread::me()
{
    return Object::me.as<State>();
}

const Thread::State &Thread::me() const
{
    return Object::me.as<State>();
}

} // namespace cc
