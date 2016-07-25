/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/GlobalCoreMutex>
#include <cc/Guard>

namespace cc {

/** \brief Thread-safe initialization of a local static variable
  *
  * Thread-safe initialization of local static variable is needed for
  * older compilers.
  */
template<class T, class Scope = None>
class LocalStatic
{
public:
    static T &instance()
    {
        Guard<SpinLock> guard(GlobalCoreMutex::instance());
        static T instance_;
        return instance_;
    }
};

template<class T, class Scope>
inline T &localStatic() { return LocalStatic<T, Scope>::instance(); }

template<class T>
inline T &localStatic() { return LocalStatic<T, None>::instance(); }

} // namespace cc
