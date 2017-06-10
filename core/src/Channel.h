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
  * \ingroup concurrency
  * \brief Inter-thread communication channel
  * \tparam T item type
  * \tparam QueueType type of queue data structure to use (e.g. PriorityQueue)
  */
template<class T, template<class> class QueueType = Queue>
class Channel: public Object
{
public:
    /** Create a new channel
      * \param capacity maximum number of items
      * \return new object instance
      */
    static Ref<Channel> create(int capacity = 0) {
        return new Channel(capacity);
    }

    /** Add a new item to the end of the queue
      * \param item item value
      * \param priority item priority
      */
    void pushBack(const T &item, int priority = 0)
    {
        Guard<Mutex> guard(mutex_);
        queue_->pushBack(item, priority);
        notEmpty_->signal();
    }

    /** Add a new item to the head of the queue
      * \param item item value
      * \param priority item priority
      */
    void pushFront(const T &item, int priority = 0)
    {
        Guard<Mutex> guard(mutex_);
        queue_->pushFront(item, priority);
        notEmpty_->signal();
    }

    /** \copydoc Queue::popBack(T *)
      */
    T popBack(T *item = 0)
    {
        Guard<Mutex> guard(mutex_);
        while (queue_->count() == 0)
            notEmpty_->wait(mutex_);
        return queue_->popBack(item);
    }

    /** \copydoc Queue::popFront(T *)
      */
    T popFront(T *item = 0)
    {
        Guard<Mutex> guard(mutex_);
        while (queue_->count() == 0)
            notEmpty_->wait(mutex_);
        return queue_->popFront(item);
    }

    /** Remove an item from the end of the queue before a certain timeout
      * \param timeout absolute timeout (e.g. System::now() + 10)
      * \param item optionally return the item value
      * \return true if an item was available before reaching the timeout, otherwise false
      */
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

    /** Remove an item from the head of the queue before a certain timeout
      * \param timeout absolute timeout (e.g. System::now() + 10)
      * \param item optionally return the item value
      * \return true if an item was available before reaching the timeout, otherwise false
      */
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

    /// \copydoc pushBack()
    inline void push(const T &item, int priority = 0) { return pushBack(item, priority); }

    /// \copydoc popFront()
    inline T pop(T *item = 0) { return popFront(item); }

    /// \copydoc popFrontBefore()
    inline bool popBefore(double timeout, T *item = 0) { return popFrontBefore(timeout, item); }

    /// Return number of queued items
    inline int count() const { Guard<Mutex> guard(mutex_); return queue_->count(); }

protected:
    Channel(int capacity):
        queue_(QueueType<T>::create(capacity)),
        mutex_(Mutex::create()),
        notEmpty_(WaitCondition::create())
    {}

private:
    Ref< QueueType<T> > queue_;
    Ref<Mutex> mutex_;
    Ref<WaitCondition> notEmpty_;
};

} // namespace cc
