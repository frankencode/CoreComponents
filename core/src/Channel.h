/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/Queue>
#include <cc/Mutex>
#include <cc/Guard>
#include <cc/WaitCondition>

namespace cc {

/** \class Channel Channel.h cc/Channel
  * \brief Inter-thread communication channel
  * \see Queue, PriorityQueue
  */
template<class T, template<class> class QueueType = Queue>
class Channel: public Object
{
public:
    static Ref<Channel> create() {
        return new Channel;
    }

    void pushBack(const T &item, int priority = 0)
    {
        Guard<Mutex> guard(mutex_);
        queue_->pushBack(item, priority);
        notEmpty_->signal();
    }

    void pushFront(const T &item, int priority = 0)
    {
        Guard<Mutex> guard(mutex_);
        queue_->pushFront(item, priority);
        notEmpty_->signal();
    }

    T popBack(T *item = 0)
    {
        Guard<Mutex> guard(mutex_);
        while (queue_->count() == 0)
            notEmpty_->wait(mutex_);
        return queue_->popBack(item);
    }

    T popFront(T *item = 0)
    {
        Guard<Mutex> guard(mutex_);
        while (queue_->count() == 0)
            notEmpty_->wait(mutex_);
        return queue_->popFront(item);
    }

    bool popBackBefore(double timeout, T *item)
    {
        Guard<Mutex> guard(mutex_);
        while (queue_->count() == 0) {
            if (!notEmpty_->waitUntil(timeout, mutex_))
                return false;
        }
        queue_->popBack(item);
        return true;
    }

    bool popFrontBefore(double timeout, T *item)
    {
        Guard<Mutex> guard(mutex_);
        while (queue_->count() == 0) {
            if (!notEmpty_->waitUntil(timeout, mutex_))
                return false;
        }
        queue_->popFront(item);
        return true;
    }

    inline void push(const T &item, int priority = 0) { return pushBack(item, priority); }
    inline T pop(T *item = 0) { return popFront(item); }

    inline bool popBefore(double timeout, T *item = 0) { return popFrontBefore(timeout, item); }

    inline int count() const { Guard<Mutex> guard(mutex_); return queue_->count(); }

protected:
    Channel():
        queue_(QueueType<T>::create()),
        mutex_(Mutex::create()),
        notEmpty_(WaitCondition::create())
    {}

private:
    Ref< QueueType<T> > queue_;
    Ref<Mutex> mutex_;
    Ref<WaitCondition> notEmpty_;
};

} // namespace cc
