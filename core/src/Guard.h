/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUX_GUARD_H
#define FLUX_GUARD_H

namespace flux {

template<class Mutex>
class Guard
{
public:
    Guard(Mutex *mutex): mutex_(mutex) { mutex_->acquire(); }
    ~Guard() { mutex_->release(); }
private:
    Mutex *mutex_;
};

} // namespace flux

#endif // FLUX_GUARD_H
