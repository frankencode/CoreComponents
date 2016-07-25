/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/System>
#include <cc/Random>

namespace cc {

Random::Random(int seed):
    x_(seed)
{
    if (seed < 0) {
        while (x_ <= 1)
            x_ = int(System::now() * 1e9);
    }
    else {
        x_ = seed + 2;
        if (x_ <= 1) x_ += 2; // overflow check
    }
}

} // namespace cc
