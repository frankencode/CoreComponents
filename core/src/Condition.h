/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUX_CONDITION_H
#define FLUX_CONDITION_H

#include <flux/Mutex>

namespace flux {

/*! \brief Wait condition
 * \sa Channel
 */
class Condition: public Object
{
public:
    inline static Ref<Condition> create() { return new Condition; }
    ~Condition();
    void wait(Mutex *mutex);
    bool waitUntil(double timeout, Mutex *mutex);
    void signal();
    void broadcast();

private:
    Condition();

    pthread_cond_t cond_;
};

} // namespace flux

#endif // FLUX_CONDITION_H
