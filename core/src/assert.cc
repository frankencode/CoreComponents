/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/exceptions>
#include <cc/assert>

namespace cc {

void throwDebugError(const char *reason, const char *source, int line)
{
    throw DebugError{reason, source, line};
}

} // namespace cc
