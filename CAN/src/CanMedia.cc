/*
 * Copyright (C) 2019-2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the Apache License version 2.0
 * (see CoreComponents/LICENSE-Apache-2.0).
 *
 */

#include <cc/CanMedia>
#include <cc/System>

namespace cc {

bool CanMedia::waitUntil(double time)
{
    double now = System::now();
    if (time <= now) return false;
    return wait(static_cast<int>((now - time) * 1000));
}

double CanMedia::State::time() const
{
    return System::now();
}

} // namespace cc
