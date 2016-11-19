/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/Mutex>

namespace cc {

/** \class WaitCondition WaitCondition.h cc/WaitCondition
  * \ingroup threading
  * \brief Wait condition
  * \see Channel
  */
class WaitCondition: public Object
{
public:
    /** Create a new wait condition
      * \return new object instance
      */
    inline static Ref<WaitCondition> create() { return new WaitCondition; }

    /** Wait for a certain condition
      * \param mutex mutex protecting the condition state
      */
    void wait(Mutex *mutex);

    /** Wait for a certain condition up to a certain timeout
      * \param timeout maximum timeout
      * \param mutex mutex protecting the condition state
      * \see System::now()
      */
    bool waitUntil(double timeout, Mutex *mutex);

    /// Wakup one waiting thread
    void signal();

    /// Wakup all waiting threads
    void broadcast();

private:
    WaitCondition();
    ~WaitCondition();

    pthread_cond_t cond_;
};

} // namespace cc
