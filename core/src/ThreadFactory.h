/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUX_THREADFACTORY_H
#define FLUX_THREADFACTORY_H

#include <flux/generics>
#include <flux/Thread>

namespace flux {

/** \brief Child thread factory
  * \see ProcessFactory
  */
class ThreadFactory: public Object
{
public:
    inline static Ref<ThreadFactory> create(Ref< Clonable<Thread> > prototype = 0) {
        return new ThreadFactory(prototype);
    }
    ~ThreadFactory();

    int stackSize() const;
    void setStackSize(int value);

    int guardSize() const;

    int detachState() const;
    void setDetachState(int value);

    pthread_attr_t *attr();

    Ref<Thread> produce();
    void start(Thread *thread);

protected:
    ThreadFactory(Ref< Clonable<Thread> > prototype = 0);

private:
    friend class ByteArray;

    Ref<ByteArray> allocateStack() const;
    static void freeStack(ByteArray *stack);
    static void *bootstrap(void *self);

    Ref< Clonable<Thread> > prototype_;
    pthread_attr_t attr_;
    int stackSize_;
    int guardSize_;
};

} // namespace flux

#endif // FLUX_THREADFACTORY_H
