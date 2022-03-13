/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Random>
#include <time.h> // clock(2)

namespace cc {

Random::Random(int seed):
    x_{static_cast<unsigned>(seed)}
{
    if (seed < 0) {
        while (x_ <= 1)
            x_ = static_cast<int>(::clock());
    }
    else {
        x_ = seed + 2;
        if (x_ <= 1) x_ += 2; // overflow check
    }
}

} // namespace cc
