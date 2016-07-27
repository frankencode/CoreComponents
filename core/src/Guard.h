/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

namespace cc {

/** \class Guard Guard.h cc/Guard
  * \brief %Mutex guard
  * \see FileLock, Mutex
  */
template<class Mutex>
class Guard
{
public:
    /** Lowlevel constructor: acquire mutex
      * \param mutex Mutex to hold
      */
    Guard(Mutex *mutex): mutex_(mutex) { mutex_->acquire(); }

    /** Lowlevel destructor: release mutex
      */
    ~Guard() { mutex_->release(); }

private:
    Mutex *mutex_;
};

} // namespace cc
