/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <sys/mman.h>
#include <flux/System>
#include <flux/exceptions>
#include <flux/ThreadFactory>

namespace flux {

ThreadFactory::ThreadFactory(Ref< Clonable<Thread> > prototype)
    : prototype_(prototype),
      stackSize_(1 << 20),
      guardSize_(System::pageSize())
{
    int ret = pthread_attr_init(&attr_);
    if (ret != 0) FLUX_SYSTEM_DEBUG_ERROR(ret);
}

ThreadFactory::~ThreadFactory()
{
    int ret = pthread_attr_destroy(&attr_);
    if (ret != 0) FLUX_SYSTEM_DEBUG_ERROR(ret);
}

int ThreadFactory::stackSize() const
{
    return stackSize_;
}

void ThreadFactory::setStackSize(int value)
{
    stackSize_ = value;
}

int ThreadFactory::guardSize() const
{
    return guardSize_;
}

int ThreadFactory::detachState() const
{
    int value = 0;
    int ret = pthread_attr_getdetachstate(&attr_, &value);
    if (ret != 0) FLUX_SYSTEM_DEBUG_ERROR(ret);
    return value;
}

void ThreadFactory::setDetachState(int value)
{
    int ret = pthread_attr_setdetachstate(&attr_, value);
    if (ret != 0) FLUX_SYSTEM_DEBUG_ERROR(ret);
}

pthread_attr_t *ThreadFactory::attr() { return &attr_; }

Ref<Thread> ThreadFactory::produce()
{
    Ref<Thread> thread = prototype_->clone();
    start(thread);
    return thread;
}

void ThreadFactory::start(Thread *thread)
{
    thread->stack_ = allocateStack();
    int ret = pthread_attr_setstack(&attr_, thread->stack_->bytes() + guardSize_, thread->stack_->count() - 2 * guardSize_);
    if (ret != 0) FLUX_SYSTEM_DEBUG_ERROR(ret);
    ret = pthread_create(&thread->tid_, &attr_, &bootstrap, static_cast<void *>(thread));
    if (ret != 0) FLUX_SYSTEM_DEBUG_ERROR(ret);
}

class CallStack: public ByteArray
{
public:
    CallStack(char *data, int size):
        ByteArray(data, size, ThreadFactory::freeStack)
    {}
};

Ref<ByteArray> ThreadFactory::allocateStack() const
{
    #ifndef MAP_ANONYMOUS
    #define MAP_ANONYMOUS MAP_ANON
    #endif
    void *protection = ::mmap(0, stackSize_, PROT_NONE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
    if (protection == MAP_FAILED) FLUX_SYSTEM_DEBUG_ERROR(errno);
    void *stack = ::mmap((char *)protection + guardSize_, stackSize_ - 2 * guardSize_, PROT_READ|PROT_WRITE, MAP_FIXED|MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
    if (stack == MAP_FAILED) FLUX_SYSTEM_DEBUG_ERROR(errno);
    return new CallStack((char *)protection, stackSize_);
}

void ThreadFactory::freeStack(ByteArray *stack)
{
    if (::munmap(stack->bytes(), stack->count()) == -1)
        FLUX_SYSTEM_DEBUG_ERROR(errno);
}

void *ThreadFactory::bootstrap(void *self)
{
    Thread *thread = static_cast<Thread *>(self);
    Thread::self_ = thread;
    thread->run();
    Thread::self_ = 0;
    return (void *)thread;
}

} // namespace flux
