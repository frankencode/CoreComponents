/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include "SystemLogStream.h"

namespace ccnode {

Ref<SystemLogStream> SystemLogStream::open(int priority)
{
    return new SystemLogStream(priority);
}

SystemLogStream::SystemLogStream(int priority):
    priority_(priority)
{}

void SystemLogStream::write(const CharArray *data)
{
    syslog(priority_, "%s", data->chars());
}

void SystemLogStream::write(const StringList *parts)
{
    write(parts->join());
}

} // namespace ccnode
