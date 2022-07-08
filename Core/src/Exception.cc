/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
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
