/*
 * Copyright (C) 2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/node/AccessLog>
#include <cc/ThreadLocalSingleton>

namespace cc {
namespace node {

AccessLog *AccessLog::instance()
{
    return ThreadLocalSingleton<AccessLog>::instance();
}

}} // namespace cc::node
