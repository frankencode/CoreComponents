/*
 * Copyright (C) 2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ThreadLocalSingleton>
#include "AccessLog.h"

namespace ccnode {

AccessLog *AccessLog::instance()
{
    return ThreadLocalSingleton<AccessLog>::instance();
}

} // namespace ccnode
