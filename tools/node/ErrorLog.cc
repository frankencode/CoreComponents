/*
 * Copyright (C) 2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ThreadLocalSingleton>
#include "ErrorLog.h"

namespace ccnode {

ErrorLog *ErrorLog::instance()
{
    return ThreadLocalSingleton<ErrorLog>::instance();
}

} // namespace ccnode
