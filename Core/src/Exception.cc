/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the Apache License version 2.0
 * (see CoreComponents/LICENSE-Apache-2.0).
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
