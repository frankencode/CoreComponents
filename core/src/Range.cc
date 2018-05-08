/*
 * Copyright (C) 2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/str>
#include <cc/Range>

namespace cc {

String str(Range range)
{
    return str(range->i0()) + ".." + str(range->i1());
}

} // namespace cc
