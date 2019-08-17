/*
 * Copyright (C) 2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/node/ErrorLog>
#include <cc/ThreadLocalSingleton>

namespace cc {
namespace node {

ErrorLog *ErrorLog::instance()
{
    return ThreadLocalSingleton<ErrorLog>::instance();
}

}} // namespace cc::node
