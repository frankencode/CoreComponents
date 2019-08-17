/*
 * Copyright (C) 2007-2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/node/SystemLogStream>

namespace cc {
namespace node {

Ref<SystemLogStream> SystemLogStream::open(int priority)
{
    return new SystemLogStream(priority);
}

SystemLogStream::SystemLogStream(int priority):
    priority_{priority}
{}

void SystemLogStream::write(const CharArray *data)
{
    syslog(priority_, "%s", data->chars());
}

void SystemLogStream::write(const StringList *parts)
{
    write(parts->join());
}

}} // namespace cc::node
