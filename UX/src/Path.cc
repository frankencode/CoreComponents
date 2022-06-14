/*
 * Copyright (C) 2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Path>
#include <cc/CubicBezier>

namespace cc {

double Path::Curve::State::length() const
{
    return CubicBezier{a_, b_, c_, d_}.length();
}

} // namespace cc
