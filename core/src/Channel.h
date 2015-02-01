/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUX_CHANNEL_H
#define FLUX_CHANNEL_H

#include <flux/Queue>
#include <flux/Mutex>
#include <flux/Guard>
#include <flux/Condition>

namespace flux {

/** \brief Inter-thread communication channel
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

    T waitNext(T *item = 0)
    {
        Guard<Mutex> guard(mutex_);
        while (queue_->count() == 0)
            notEmpty_->wait(mutex_);
        T h = queue_->front();
        if (item) *item = h;
        return h;
    }

protected:
    Channel()
        : queue_(QueueType<T>::create()),
          mutex_(Mutex::create()),
          notEmpty_(Condition::create())
    {}

private:
    Ref< QueueType<T> > queue_;
    Ref<Mutex> mutex_;
    Ref<Condition> notEmpty_;
};

} // namespace flux

#endif // FLUX_CHANNEL_H
