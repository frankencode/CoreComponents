/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Exception>

namespace cc {

const char* Exception::what() const throw()
{
    static thread_local String m = message();
    return m;
}

} // namespace cc
