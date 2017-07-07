/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/Object>
#include <cc/SpinLock>
#include <cc/Ref>

namespace cc {

class GlobalCoreMutex;

class GlobalCoreMutexInitializer
{
public:
    GlobalCoreMutexInitializer();
private:
    static int count_;
};

namespace { GlobalCoreMutexInitializer globalCoreMutexInitializer; }

class GlobalCoreMutex: public SpinLock
{
public:
    static GlobalCoreMutex *instance();

private:
    GlobalCoreMutex() {}
};

} // namespace cc
