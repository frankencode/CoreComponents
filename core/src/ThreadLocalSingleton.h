/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/Object>
#include <cc/Ref>

namespace cc {

/** \class ThreadLocalSingleton ThreadLocalSingleton.h cc/ThreadLocalSingleton
  */
template<class SubClass>
class ThreadLocalSingleton
{
public:
    static SubClass *instance()
    {
        static thread_local Ref<SubClass> instance_(new SubClass);
        return instance_;
    }
};

} // namespace cc
