/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUX_WAITCONDITION_H
#define FLUX_WAITCONDITION_H

#include <flux/Mutex>

namespace flux {

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

} // namespace flux

#endif // FLUX_WAITCONDITION_H
