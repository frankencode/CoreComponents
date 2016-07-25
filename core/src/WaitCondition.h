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

/** \brief Wait condition
  * \see Channel
  */
class WaitCondition: public Object
{
public:
    inline static Ref<WaitCondition> create() { return new WaitCondition; }
    ~WaitCondition();
    void wait(Mutex *mutex);
    bool waitUntil(double timeout, Mutex *mutex);
    void signal();
    void broadcast();

private:
    WaitCondition();

    pthread_cond_t cond_;
};

} // namespace cc

