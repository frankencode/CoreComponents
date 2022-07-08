/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/Random>
#include <time.h> // clock(2)

namespace cc {

Random::Random(int seed):
    x_{static_cast<uint32_t>(seed)}
{
    if (seed < 0) {
        while (x_ <= 1) {
            x_ = static_cast<uint32_t>(::clock());
        }
    }
    else {
        x_ = seed + 2;
        if (x_ <= 1) x_ += 2; // overflow check
    }
}

} // namespace cc
