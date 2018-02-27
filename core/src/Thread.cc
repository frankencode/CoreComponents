/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <sys/mman.h>
#include <cc/strings>
#include <cc/exceptions>
#include <cc/WaitCondition>
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

class ThreadStack: public CharArray
{
public:
    ThreadStack(char *data, int size):
        CharArray(data, size, freeStack)
    {}

    static void freeStack(CharArray *stack) {
        if (::munmap(stack->bytes(), stack->count()) == -1)
            CC_SYSTEM_DEBUG_ERROR(errno);
    }
};

Ref<CharArray> Thread::allocateStack(int stackSize, int guardSize)
{
    #ifndef MAP_ANONYMOUS
    #define MAP_ANONYMOUS MAP_ANON
    #endif
    #ifndef MAP_GROWSDOWN
    #define MAP_GROWSDOWN 0
    #endif
    void *protection = ::mmap(0, stackSize, PROT_NONE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
    if (protection == MAP_FAILED) CC_SYSTEM_DEBUG_ERROR(errno);
    void *stack = ::mmap((char *)protection + guardSize, stackSize - 2 * guardSize, PROT_READ|PROT_WRITE, MAP_FIXED|MAP_PRIVATE|MAP_ANONYMOUS|MAP_GROWSDOWN, -1, 0);
    if (stack == MAP_FAILED) CC_SYSTEM_DEBUG_ERROR(errno);
    return new ThreadStack((char *)protection, stackSize);
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
        int ret = pthread_attr_setstacksize(&attr, stackSize_);
        if (ret != 0) CC_SYSTEM_DEBUG_ERROR(ret);
    }

    /*{ // setting a custom stack became disfunctional with some glibc versions
        const int guardSize = System::pageSize();
        stack_ = allocateStack(stackSize_, guardSize);
        int ret = pthread_attr_setstack(&attr, stack_->bytes() + guardSize, stack_->count() - 2 * guardSize);
        if (ret != 0) CC_SYSTEM_DEBUG_ERROR(ret);
    }*/

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
    sleepUntil(System::now() + duration);
}

void Thread::sleepUntil(double timeout)
{
    Ref<Mutex> mutex = Mutex::create();
    Ref<WaitCondition> condition = WaitCondition::create();
    mutex->acquire();
    condition->waitUntil(timeout, mutex);
    mutex->release();
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
