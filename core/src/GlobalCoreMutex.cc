/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/GlobalCoreMutex>

namespace cc {

int GlobalCoreMutexInitializer::count_ = 0;

GlobalCoreMutexInitializer::GlobalCoreMutexInitializer()
{
    if (count_ == 0) {
        ++count_;
        GlobalCoreMutex::instance();
    }
}

GlobalCoreMutex *GlobalCoreMutex::instance()
{
    static GlobalCoreMutex instance_;
    return &instance_;
}

} // namespace cc
