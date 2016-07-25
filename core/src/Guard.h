/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

namespace cc {

/** \brief %Mutex guard
  * \see FileLock, Mutex
  */
template<class Mutex>
class Guard
{
public:
    Guard(Mutex *mutex): mutex_(mutex) { mutex_->acquire(); }
    ~Guard() { mutex_->release(); }
private:
    Mutex *mutex_;
};

} // namespace cc
