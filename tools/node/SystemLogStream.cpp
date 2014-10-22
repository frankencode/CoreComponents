/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include "SystemLogStream.h"

namespace fluxnode {

Ref<SystemLogStream> SystemLogStream::open(int priority)
{
    return new SystemLogStream(priority);
}

SystemLogStream::SystemLogStream(int priority):
    priority_(priority)
{}

int SystemLogStream::read(ByteArray *buf)
{
    return 0;
}

void SystemLogStream::write(const ByteArray *buf)
{
    syslog(priority_, "%s", buf->chars());
}

void SystemLogStream::write(const StringList *parts)
{
    write(parts->join());
}

} // namespace fluxnode
